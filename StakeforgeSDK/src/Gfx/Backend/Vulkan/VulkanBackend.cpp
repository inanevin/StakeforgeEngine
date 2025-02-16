/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Gfx/Backend/Vulkan/VulkanBackend.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanFunctions.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Memory/PoolAllocator.hpp"

#define VMA_IMPLEMENTATION
#include "SFG/Vendor/vulkan/vk_mem_alloc.h"

#include "SFG/Vendor/vulkan/VkBootstrap.h"
#include "SFG/Gfx/Backend/Vulkan/VulkanQueue.hpp"

#include <algorithm>
#include <vulkan/vulkan.h>

namespace SFG
{

#ifdef SFG_DEBUG

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		switch (messageSeverity)
		{
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			SFG_ERR("Vulkan: {0}", pCallbackData->pMessage);
			break;
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			SFG_WARN("Vulkan: {0}", pCallbackData->pMessage);
			break;
		default:
			SFG_INFO("Vulkan: {0}", pCallbackData->pMessage);
			break;
		}
		return VK_FALSE;
	}
#endif

#define VK_CHECK_RESULT(item, err)                                                                                                                                                                                                                                 \
	{                                                                                                                                                                                                                                                              \
		SFG_ASSERT(item == VK_SUCCESS, "Vulkan operation failed: {0}", item);                                                                                                                                                                                      \
		if (item != VK_SUCCESS)                                                                                                                                                                                                                                    \
			throw std::runtime_error(err);                                                                                                                                                                                                                         \
	}

	void VulkanBackend::Create(String& errString)
	{
		SFG_INFO("************* Initializing Vulkan backend... *************");
		constexpr uint32 VK_MAJOR = 1;
		constexpr uint32 VK_MINOR = 2;

		/*
			Select extensions and create instance.
		*/
		vkb::InstanceBuilder builder;
		builder = builder.set_app_name(SFG_APPNAME).require_api_version(VK_MAJOR, VK_MINOR, 0);

#ifdef SFG_DEBUG
		constexpr uint8 EXTENSION_COUNT						= 3;
		const char*		requiredExtensions[EXTENSION_COUNT] = {"VK_KHR_surface", "VK_EXT_validation_features", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
		builder.enable_validation_layers(true).request_validation_layers(true);

#else
		constexpr uint8 EXTENSION_COUNT						= 1;
		const char*		requiredExtensions[EXTENSION_COUNT] = {"VK_KHR_surface"};
		builder.enable_validation_layers(false).request_validation_layers(false);
#endif

		for (uint8 i = 0; i < EXTENSION_COUNT; i++)
			builder.enable_extension(requiredExtensions[i]);

#ifdef SFG_DEBUG
		VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		builder.set_debug_callback(VkDebugCallback);
		builder.set_debug_messenger_severity(severity);
		builder.set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT);
#endif

		auto buildResult = builder.build();
		if (!buildResult.has_value())
		{
			errString = "Vulkan: Builder failed " + buildResult.error().message();
			return;
		}

		/*
			Check physical devices and report.
		*/
		vkb::Instance vkbInst = buildResult.value();
		m_instance			  = vkbInst.instance;
		m_debugMessenger	  = vkbInst.debug_messenger;

		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(vkbInst.instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			errString = "Vulkan: No physical devices were found.";
			return;
		}

		{
			Vector<VkPhysicalDevice> devices(deviceCount);
			VkResult				 result = vkEnumeratePhysicalDevices(vkbInst.instance, &deviceCount, devices.data());

			for (VkPhysicalDevice device : devices)
			{
				VkPhysicalDeviceProperties props = {};
				vkGetPhysicalDeviceProperties(device, &props);
				SFG_INFO("Vulkan: Found device: {0} DeviceID: {1}", props.deviceName, props.deviceID);
			}
		}

		/*
			Select physical device, setup required device features and vulkan features.
		*/
		VkPhysicalDeviceFeatures requiredDeviceFeatures = {
			.depthClamp		= true,
			.depthBiasClamp = true,
		};

		VkPhysicalDeviceVulkan12Features requiredVulkan12Features = {};
		requiredVulkan12Features.sType							  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		requiredVulkan12Features.pNext							  = nullptr;
		requiredVulkan12Features.timelineSemaphore				  = true;

		vkb::PhysicalDeviceSelector selector{vkbInst};
		selector.set_minimum_version(VK_MAJOR, VK_MINOR).defer_surface_initialization().prefer_gpu_device_type(vkb::PreferredDeviceType::discrete);
		selector.set_required_features(requiredDeviceFeatures);
		selector.set_required_features_12(requiredVulkan12Features);
		selector.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

		vkb::Result<vkb::PhysicalDevice> physicalDeviceResult = selector.select(vkb::DeviceSelectionMode::only_fully_suitable);
		if (!physicalDeviceResult.has_value())
		{
			errString = "Vulkan: Could not find a device that fits the required Vulkan features!";
			return;
		}

		vkb::PhysicalDevice physicalDevice = physicalDeviceResult.value();

		/*
			Report the device and check properteis.
		*/
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(physicalDevice.physical_device, &gpuProperties);
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice.physical_device, &memoryProperties);

		VkPhysicalDeviceFeatures features = {};
		vkGetPhysicalDeviceFeatures(physicalDevice.physical_device, &features);

		SFG_INFO("Vulkan: Selected device: {0}", gpuProperties.deviceName);
		SFG_INFO("Vulkan: API Version: {0}.{1}.{2}", VK_VERSION_MAJOR(gpuProperties.apiVersion), VK_VERSION_MINOR(gpuProperties.apiVersion), VK_VERSION_PATCH(gpuProperties.apiVersion));
		SFG_INFO("Vulkan: Memory Heaps: {0}", memoryProperties.memoryHeapCount);
		for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++)
		{
			VkDeviceSize heapSizeMB	   = memoryProperties.memoryHeaps[i].size / (1024 * 1024);
			bool		 isDeviceLocal = memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;

			SFG_INFO("Vulkan: Heap {0}: {1} MB {2}", i, heapSizeMB, isDeviceLocal ? "(Device Local - VRAM)" : "(Non-Device Local - Shared)");
		}

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			VkMemoryPropertyFlags flags		= memoryProperties.memoryTypes[i].propertyFlags;
			uint32_t			  heapIndex = memoryProperties.memoryTypes[i].heapIndex;

			std::string properties;
			if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				properties += "DeviceLocal ";
			if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				properties += "HostVisible ";
			if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				properties += "HostCoherent ";
			if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
				properties += "HostCached ";
			if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
				properties += "LazilyAllocated ";

			SFG_INFO("Vulkan: Memory Type {0}: Heap {1} | {2}", i, heapIndex, properties);

			if ((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) && (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
			{
				const bool isCoherent = (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
				SFG_INFO("CPU visible GPU memory is available! Is Coherent: {0}", isCoherent);
				m_cpuVisibleGPUMemoryType = isCoherent ? CPUVisibleGPUMemoryType::Coherent : CPUVisibleGPUMemoryType::NonCoherent;
			}
		}

		/*
			Create device, manually enable required extensions.
		*/
		vkb::DeviceBuilder deviceBuilder{physicalDevice};

		/* Enable requested extension */
		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = VkPhysicalDeviceDynamicRenderingFeaturesKHR{
			.sType			  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
			.pNext			  = nullptr,
			.dynamicRendering = VK_TRUE,
		};
		deviceBuilder.add_pNext(&dynamicRenderingFeature);

		vkb::Result deviceBuilderResult = deviceBuilder.build();

		if (!deviceBuilderResult.has_value())
		{
			errString = "Vulkan: Could not build the device!";
			return;
		}

		vkb::Device vkbDevice = deviceBuilderResult.value();
		m_device			  = vkbDevice.device;
		m_gfxQueue			  = vkbDevice.get_queue(vkb::QueueType::graphics).value();

		/*
			Queue checkup.
		*/
		Vector<VkQueueFamilyProperties> queueFamilies = physicalDevice.get_queue_families();
		std::sort(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& a, const VkQueueFamilyProperties& b) {
			return a.queueCount > b.queueCount; // Prefer more queues
		});

		auto itDedicatedTransfer = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(qf.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT); });
		if (itDedicatedTransfer != queueFamilies.end())
		{
			m_transferQueueFamilyIndex	= static_cast<int32>(std::distance(queueFamilies.begin(), itDedicatedTransfer));
			m_transferQueueAvailability = QueueAvailabilityType::Dedicated;
			SFG_INFO("Vulkan: Found dedicated transfer queue.");
		}
		else
		{
			auto itSeparateTransfer = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT); });
			if (itSeparateTransfer != queueFamilies.end())
			{
				m_transferQueueFamilyIndex	= static_cast<int32>(std::distance(queueFamilies.begin(), itSeparateTransfer));
				m_transferQueueAvailability = QueueAvailabilityType::Separate;
				SFG_INFO("Vulkan: Found separate transfer queue.");
			}
			else
			{
				auto itAnyTransfer = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_TRANSFER_BIT); });
				SFG_ASSERT(itAnyTransfer != queueFamilies.end());
				m_transferQueueFamilyIndex = static_cast<int32>(std::distance(queueFamilies.begin(), itAnyTransfer));
				SFG_INFO("Vulkan: Transfer queue is graphics queue.");
			}
		}

		vkGetDeviceQueue(vkbDevice.device, m_transferQueueFamilyIndex, 0, &m_transferQueue);

		auto itDedicatedCompute = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(qf.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT); });
		if (itDedicatedCompute != queueFamilies.end())
		{
			m_computeQueueFamilyIndex  = static_cast<int32>(std::distance(queueFamilies.begin(), itDedicatedCompute));
			m_computeQueueAvailability = QueueAvailabilityType::Dedicated;
			SFG_INFO("Vulkan: Found dedicated compute queue.");
		}
		else
		{
			auto itSeparateCompute = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT); });
			if (itSeparateCompute != queueFamilies.end())
			{
				m_computeQueueFamilyIndex  = static_cast<int32>(std::distance(queueFamilies.begin(), itSeparateCompute));
				m_computeQueueAvailability = QueueAvailabilityType::Separate;
				SFG_INFO("Vulkan: Found separate compute queue.");
			}
			else
			{
				auto itAnyCompute = std::ranges::find_if(queueFamilies, [](const VkQueueFamilyProperties& qf) { return (qf.queueFlags & VK_QUEUE_COMPUTE_BIT); });
				SFG_ASSERT(itAnyCompute != queueFamilies.end());
				m_computeQueueFamilyIndex = static_cast<int32>(std::distance(queueFamilies.begin(), itAnyCompute));
				SFG_INFO("Vulkan: Compute queue is graphics queue.");
			}
		}

		vkGetDeviceQueue(vkbDevice.device, m_computeQueueFamilyIndex, 0, &m_computeQueue);
		vkGetDeviceQueue(vkbDevice.device, m_transferQueueFamilyIndex, 0, &m_transferQueue);

		/* Feature function pointers */
		pfn_cmdBeginRendering		   = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdBeginRenderingKHR"));
		pfn_cmdEndRendering			   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(m_device, "vkCmdEndRenderingKHR"));
		pfn_cmdBeginDebugUtilsLabel	   = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(m_device, "vkCmdBeginDebugUtilsLabelEXT"));
		pfn_cmdEndDebugUtilsLabel	   = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(m_device, "vkCmdEndDebugUtilsLabelEXT"));
		pfn_setDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT"));

		if (!pfn_cmdBeginRendering || !pfn_cmdEndRendering)
		{
			errString = "Failed to load vkCmdBeginRenderingKHR or vkCmdEndRenderingKHR!";
			return;
		}

#ifdef SFG_DEBUG
		if (!pfn_setDebugUtilsObjectNameEXT || !pfn_cmdBeginDebugUtilsLabel || !pfn_cmdEndDebugUtilsLabel)
		{
			errString = "Failed to load vkSetDebugUtilsObjectNameEXT or vkCmdBeginDebugUtilsLabelEXT or vkCmdEndDebugUtilsLabelEXT!";
			return;
		}
#endif

		/* VMA */
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice		 = physicalDevice.physical_device;
		allocatorInfo.device				 = m_device;
		allocatorInfo.instance				 = m_instance;
		vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);

		SFG_INFO("************* Successfuly initialized Vulkan backend. *************");
	}

	void VulkanBackend::Destroy()
	{
		if (m_device)
			vkDestroyDevice(m_device, m_allocationCallbacks);

		if (m_debugMessenger)
			vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);

		if (m_instance)
			vkDestroyInstance(m_instance, m_allocationCallbacks);

		if (m_vmaAllocator)
			vmaDestroyAllocator(m_vmaAllocator);
	}

} // namespace SFG

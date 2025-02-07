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

#include "SFG/Gfx/Backend/Vulkan/VkBackend.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/Vendor/Vulkan/VkBootstrap.h"
#include "SFG/IO/Log.hpp"
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

	struct VulkanBackendUserData
	{
		VkInstance				 instance			 = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger		 = nullptr;
		VkAllocationCallbacks*	 allocationCallbacks = nullptr;
		VkDevice				 device				 = nullptr;

		// Function pointers
		PFN_vkCmdBeginRenderingKHR		 vkCmdBeginRenderingKHR		  = nullptr;
		PFN_vkCmdEndRenderingKHR		 vkCmdEndRenderingKHR		  = nullptr;
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
		PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
		PFN_vkCmdEndDebugUtilsLabelEXT	 vkCmdEndDebugUtilsLabelEXT	  = nullptr;
	};

	static VulkanBackendUserData g_vkUserData = {};

	void VkBackend::Create(String& errString)
	{

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
		vkb::Instance vkbInst		= buildResult.value();
		g_vkUserData.instance		= vkbInst.instance;
		g_vkUserData.debugMessenger = vkbInst.debug_messenger;

		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(g_vkUserData.instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			errString = "Vulkan: No physical devices were found.";
			return;
		}

		{
			Vector<VkPhysicalDevice> devices(deviceCount);
			VkResult				 result = vkEnumeratePhysicalDevices(g_vkUserData.instance, &deviceCount, devices.data());

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
		g_vkUserData.device	  = vkbDevice.device;

		/* Feature function pointers */
		g_vkUserData.vkCmdBeginRenderingKHR		  = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(g_vkUserData.device, "vkCmdBeginRenderingKHR"));
		g_vkUserData.vkCmdEndRenderingKHR		  = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(g_vkUserData.device, "vkCmdEndRenderingKHR"));
		g_vkUserData.vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(g_vkUserData.device, "vkSetDebugUtilsObjectNameEXT"));
		g_vkUserData.vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(g_vkUserData.device, "vkCmdBeginDebugUtilsLabelEXT"));
		g_vkUserData.vkCmdEndDebugUtilsLabelEXT	  = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(g_vkUserData.device, "vkCmdEndDebugUtilsLabelEXT"));

		if (!g_vkUserData.vkCmdBeginRenderingKHR || !g_vkUserData.vkCmdEndRenderingKHR)
		{
			errString = "Failed to load vkCmdBeginRenderingKHR or vkCmdEndRenderingKHR!";
			return;
		}

#ifdef SFG_DEBUG
		if (!g_vkUserData.vkSetDebugUtilsObjectNameEXT || !g_vkUserData.vkCmdBeginDebugUtilsLabelEXT || !g_vkUserData.vkCmdEndDebugUtilsLabelEXT)
		{
			errString = "Failed to load vkSetDebugUtilsObjectNameEXT or vkCmdBeginDebugUtilsLabelEXT or vkCmdEndDebugUtilsLabelEXT!";
			return;
		}
#endif
	}

	void VkBackend::Destroy()
	{
		if (g_vkUserData.device)
			vkDestroyDevice(g_vkUserData.device, g_vkUserData.allocationCallbacks);

		if (g_vkUserData.debugMessenger)
			vkb::destroy_debug_utils_messenger(g_vkUserData.instance, g_vkUserData.debugMessenger);

		if (g_vkUserData.instance)
			vkDestroyInstance(g_vkUserData.instance, g_vkUserData.allocationCallbacks);
	}
} // namespace SFG

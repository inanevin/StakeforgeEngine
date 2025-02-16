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

#include "SFG/Gfx/Backend/Vulkan/VulkanSwapchain.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanUtility.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanFunctions.hpp"
#include "SFG/Gfx/Common/SwapchainDesc.hpp"
#include "SFG/IO/Assert.hpp"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "SFG/Vendor/vulkan/VkBootstrap.h"

namespace SFG
{
	void VulkanSwapchain::Create(const SwapchainDesc& desc)
	{

		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = VkWin32SurfaceCreateInfoKHR{};
		surfaceCreateInfo.sType						  = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext						  = nullptr;
		surfaceCreateInfo.hinstance					  = static_cast<HINSTANCE>(desc.osHandle);
		surfaceCreateInfo.hwnd						  = static_cast<HWND>(desc.window);
		vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface);

		const VkPresentModeKHR presentMode = desc.vsyncOn ? VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR : VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
		const VkFormat		   swpFormat   = VulkanUtility::GetVkFormat(desc.format);
		vkb::SwapchainBuilder  swapchainBuilder{m_gpu, m_device, m_surface};
		swapchainBuilder = swapchainBuilder
							   //.use_default_format_selection()
							   .set_desired_present_mode(presentMode)
							   .set_desired_extent(desc.width, desc.height)
							   .set_desired_format({swpFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
							   .set_desired_min_image_count(BACK_BUFFER_COUNT);

		vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
		m_swapchain					= vkbSwapchain.swapchain;

		const std::vector<VkImageView>& imageViews = vkbSwapchain.get_image_views().value();
		const std::vector<VkImage>&		images	   = vkbSwapchain.get_images().value();
		for (uint8 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			m_imageViews[i] = imageViews.at(i);
			m_images[i]		= images.at(i);
		}

		const VkSemaphoreCreateInfo smpInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
		};

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			const VkResult result = vkCreateSemaphore(m_device, &smpInfo, nullptr, &m_imageAcquiredSemaphores[i]);
			SFG_ASSERT(result == VK_SUCCESS);
		}

		VK_NAME_OBJECT(m_swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, desc.name, nameInfo);
	}

	void VulkanSwapchain::Destroy()
	{
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		for (uint8 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			vkDestroyImageView(m_device, m_imageViews[i], nullptr);
			m_imageViews[i] = nullptr;
		}

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_device, m_imageAcquiredSemaphores[i], nullptr);
			m_imageAcquiredSemaphores[i] = nullptr;
		}
	}

	void VulkanSwapchain::Recreate(const SwapchainDesc& desc)
	{
		const VkPresentModeKHR presentMode = desc.vsyncOn ? VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR : VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
		const VkFormat		   swpFormat   = VulkanUtility::GetVkFormat(desc.format);

		const VkSwapchainKHR  oldSwapchain = m_swapchain;
		vkb::SwapchainBuilder swapchainBuilder{m_gpu, m_device, m_surface};
		vkb::Swapchain		  vkbSwapchain = swapchainBuilder.set_old_swapchain(m_swapchain)
										  .set_desired_present_mode(presentMode)
										  .set_desired_extent(desc.width, desc.height)
										  .set_desired_format({swpFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
										  .set_required_min_image_count(BACK_BUFFER_COUNT)
										  .build()
										  .value();

		vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);

		for (uint8 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			vkDestroyImageView(m_device, m_imageViews[i], nullptr);
			m_imageViews[i] = nullptr;
		}

		const std::vector<VkImageView>& imageViews = vkbSwapchain.get_image_views().value();
		const std::vector<VkImage>&		images	   = vkbSwapchain.get_images().value();
		for (uint8 i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			m_imageViews[i] = imageViews.at(i);
			m_images[i]		= images.at(i);
		}
	}

} // namespace SFG

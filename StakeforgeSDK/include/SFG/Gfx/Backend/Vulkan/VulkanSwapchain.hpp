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

#pragma once

#include "SFG/Type/SizeDefinitions.hpp"
#include "SFG/Gfx/Common/GfxConstants.hpp"

struct VkDevice_T;
struct VkInstance_T;
struct VkPhysicalDevice_T;
struct VkSwapchainKHR_T;
struct VkSurfaceKHR_T;
struct VkImageView_T;
struct VkImage_T;
struct VkSemaphore_T;

namespace SFG
{
	struct SwapchainDesc;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain() = delete;
		VulkanSwapchain(VkInstance_T* inst, VkDevice_T* device, VkPhysicalDevice_T* gpu) : m_instance(inst), m_device(device), m_gpu(gpu){};

		/// <summary>
		///
		/// </summary>
		void Create(const SwapchainDesc& desc);

		/// <summary>
		///
		/// </summary>
		void Recreate(const SwapchainDesc& desc);

		/// <summary>
		///
		/// </summary>
		void Destroy();

	private:
		VkInstance_T*		m_instance = nullptr;
		VkDevice_T*			m_device   = nullptr;
		VkPhysicalDevice_T* m_gpu	   = nullptr;

		VkSurfaceKHR_T*	  m_surface	  = nullptr;
		VkSwapchainKHR_T* m_swapchain = nullptr;
		VkImageView_T*	  m_imageViews[BACK_BUFFER_COUNT];
		VkImage_T*		  m_images[BACK_BUFFER_COUNT];
		VkSemaphore_T*	  m_imageAcquiredSemaphores[FRAMES_IN_FLIGHT];
	};
}; // namespace SFG

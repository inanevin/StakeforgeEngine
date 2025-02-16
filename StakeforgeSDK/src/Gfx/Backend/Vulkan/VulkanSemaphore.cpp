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

#include "SFG/Gfx/Backend/Vulkan/VulkanSemaphore.hpp"
#include "SFG/IO/Assert.hpp"
#include <vulkan/vulkan.h>

namespace SFG
{
	void VulkanSemaphore::Create()
	{
		VkSemaphoreTypeCreateInfo timelineCreateInfo;
		timelineCreateInfo.sType		 = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		timelineCreateInfo.pNext		 = NULL;
		timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		timelineCreateInfo.initialValue	 = 0;

		VkSemaphoreCreateInfo info = VkSemaphoreCreateInfo{};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, info.pNext = &timelineCreateInfo;
		info.flags = 0;

		const VkResult result = vkCreateSemaphore(m_device, &info, nullptr, &m_semaphore);
		SFG_ASSERT(result == VK_SUCCESS);
	}

	void VulkanSemaphore::Destroy()
	{
		vkDestroySemaphore(m_device, m_semaphore, nullptr);
		m_semaphore = nullptr;
	}

	void VulkanSemaphore::Wait(uint64 value, uint32 sleepMS)
	{
		const uint64		timeout	 = static_cast<uint64>(5000000000);
		VkSemaphoreWaitInfo waitInfo = VkSemaphoreWaitInfo{};
		waitInfo.sType				 = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		waitInfo.pNext				 = nullptr;
		waitInfo.flags				 = 0;
		waitInfo.semaphoreCount		 = 1;
		waitInfo.pSemaphores		 = &m_semaphore;
		waitInfo.pValues			 = &value;
		const VkResult result		 = vkWaitSemaphores(m_device, &waitInfo, timeout);
		SFG_ASSERT(result == VK_SUCCESS);
	}
} // namespace SFG

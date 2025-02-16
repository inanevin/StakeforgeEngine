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
#include "SFG/Data/String.hpp"

struct VkQueue_T;
struct VkDevice_T;
struct VkInstance_T;
struct VkDebugUtilsMessengerEXT_T;
struct VkAllocationCallbacks;
struct VmaAllocator_T;
enum VkObjectType;

namespace SFG
{
	class VulkanBackend
	{
	private:
		enum class CPUVisibleGPUMemoryType
		{
			None,
			NonCoherent,
			Coherent,
		};

		enum class QueueAvailabilityType
		{
			Default,
			Dedicated,
			Separate,
		};

	public:
		/// <summary>
		///
		/// </summary>
		void Create(String& errString);

		/// <summary>
		///
		/// </summary>
		void Destroy();

	private:
		static void* s_vkCmdBeginRenderingKHR;
		static void* s_vkCmdEndRenderingKHR;
		static void* s_vkCmdBeginDebugUtilsLabelEXT;
		static void* s_vkCmdEndDebugUtilsLabelEXT;

		VmaAllocator_T*				m_vmaAllocator				= nullptr;
		VkInstance_T*				m_instance					= nullptr;
		VkDevice_T*					m_device					= nullptr;
		VkDebugUtilsMessengerEXT_T* m_debugMessenger			= nullptr;
		VkAllocationCallbacks*		m_allocationCallbacks		= nullptr;
		VkQueue_T*					m_gfxQueue					= nullptr;
		VkQueue_T*					m_transferQueue				= nullptr;
		VkQueue_T*					m_computeQueue				= nullptr;
		QueueAvailabilityType		m_transferQueueAvailability = QueueAvailabilityType::Default;
		QueueAvailabilityType		m_computeQueueAvailability	= QueueAvailabilityType::Default;
		CPUVisibleGPUMemoryType		m_cpuVisibleGPUMemoryType	= CPUVisibleGPUMemoryType::None;
		int32						m_transferQueueFamilyIndex	= -1;
		int32						m_computeQueueFamilyIndex	= -1;
	};
}; // namespace SFG
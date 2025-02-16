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

#include "SFG/Gfx/Backend/Vulkan/VulkanResource.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanFunctions.hpp"
#include "SFG/Gfx/Common/ResourceDesc.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/Vendor/vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#
namespace SFG
{
	void VulkanResource::Create(const ResourceDesc& desc)
	{
		VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		bufferInfo.size				  = desc.size;

		if (desc.type == ResourceType::VertexBuffer)
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (desc.type == ResourceType::UBO)
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (desc.type == ResourceType::IndexBuffer)
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if (desc.type == ResourceType::IndirectBuffer)
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (desc.type == ResourceType::SSBO)
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		if (bufferInfo.usage == 0)
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo = {};

		if (desc.storageType == ResourceStorageType::HostVisibleDevice)
		{
			allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			allocInfo.usage			= VMA_MEMORY_USAGE_AUTO;
			allocInfo.flags			= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		}
		if (desc.storageType == ResourceStorageType::Device)
		{
			allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			allocInfo.usage			= VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		if (desc.storageType == ResourceStorageType::Host)
		{
			allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			allocInfo.usage			= VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			allocInfo.flags			= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

		const VkResult res = vmaCreateBuffer(m_vma, &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);
		SFG_ASSERT(res == VK_SUCCESS);
		VK_NAME_OBJECT(m_buffer, VK_OBJECT_TYPE_BUFFER, desc.name, info);
	}

	void VulkanResource::Destroy()
	{
		vmaDestroyBuffer(m_vma, m_buffer, m_allocation);
		m_buffer	 = nullptr;
		m_allocation = nullptr;
	}

	void VulkanResource::Map(uint8*& ptr)
	{
		vmaMapMemory(m_vma, m_allocation, reinterpret_cast<void**>(&ptr));
	}
	void VulkanResource::Unmap()
	{
		vmaUnmapMemory(m_vma, m_allocation);
	}
} // namespace SFG

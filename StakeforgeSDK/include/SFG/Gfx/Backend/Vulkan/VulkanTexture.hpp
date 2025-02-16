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

#include "SFG/Data/Vector.hpp"
#include "SFG/Type/SizeDefinitions.hpp"
#include "SFG/Data/DynamicArray.hpp"

struct VkDevice_T;
struct VmaAllocator_T;
struct VkImage_T;
struct VkImageView_T;
struct VmaAllocation_T;

namespace SFG
{
	struct TextureDesc;

	class VulkanTexture
	{
	public:
		VulkanTexture() = delete;
		VulkanTexture(VkDevice_T* device, VmaAllocator_T* vma) : m_device(device), m_vma(vma){};

		/// <summary>
		///
		/// </summary>
		void Create(const TextureDesc& def);

		/// <summary>
		///
		/// </summary>
		void Destroy();

	private:
		DynamicArray<VkImageView_T*> m_imageViews;
		VmaAllocator_T*				 m_vma		   = nullptr;
		VkDevice_T*					 m_device	   = nullptr;
		VkImage_T*					 m_image	   = nullptr;
		VmaAllocation_T*			 m_alloc	   = nullptr;
		uint32						 m_aspectFlags = 0;
	};
}; // namespace SFG

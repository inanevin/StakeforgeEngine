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

#include "SFG/Gfx/Backend/Vulkan/VulkanTexture.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanUtility.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanFunctions.hpp"
#include "SFG/Gfx/Common/TextureDesc.hpp"
#include "SFG/Gfx/Common/TextureView.hpp"
#include "SFG/Gfx/Common/TextureFlags.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/Vendor/vulkan/vk_mem_alloc.h"

namespace SFG
{
	void VulkanTexture::Create(const TextureDesc& desc)
	{
		VkSampleCountFlagBits aq;

		VkImageCreateInfo imgCreateInfo = VkImageCreateInfo{};
		imgCreateInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgCreateInfo.pNext				= nullptr;
		imgCreateInfo.imageType			= VK_IMAGE_TYPE_2D;
		imgCreateInfo.format			= VulkanUtility::GetVkFormat(desc.format);
		imgCreateInfo.extent			= VkExtent3D{desc.width, desc.height, 1};
		imgCreateInfo.mipLevels			= desc.mipLevels;
		imgCreateInfo.arrayLayers		= desc.arrayLevels;
		imgCreateInfo.samples			= VulkanUtility::GetVkSamples(desc.sampleCount);
		imgCreateInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		imgCreateInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
		imgCreateInfo.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;

		if (desc.type == TextureType::Texture1D)
			imgCreateInfo.imageType = VK_IMAGE_TYPE_1D;
		else if (desc.type == TextureType::Texture3D)
			imgCreateInfo.imageType = VK_IMAGE_TYPE_3D;

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_DEPTH_ATT)
			imgCreateInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_COLOR_ATT)
			imgCreateInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_SAMPLED)
			imgCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_TRANSFER_SRC)
			imgCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_TRANSFER_DST)
			imgCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocinfo = VmaAllocationCreateInfo{};
		allocinfo.usage					  = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocinfo.requiredFlags			  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		const VkResult res = vmaCreateImage(m_vma, &imgCreateInfo, &allocinfo, &m_image, &m_alloc, nullptr);
		SFG_ASSERT(res == VK_SUCCESS);

		if (desc.flags & TextureFlags::TEXTURE_FLAGS_DEPTH_ATT)
			m_aspectFlags |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		else if (!(desc.flags & TextureFlags::TEXTURE_FLAGS_STENCIL_ATT))
			m_aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		VK_NAME_OBJECT(m_image, VK_OBJECT_TYPE_IMAGE, desc.name, imgName);

		auto createView = [&](bool useCubeView, uint32 baseArrayLayer, uint32 layerCount, uint32 baseMipLevel, uint32 mipCount, VkImageView& imgView) {
			VkImageSubresourceRange subResRange = VkImageSubresourceRange{};
			subResRange.aspectMask				= (VkImageAspectFlags)m_aspectFlags;
			subResRange.baseMipLevel			= baseMipLevel;
			subResRange.levelCount				= mipCount;
			subResRange.baseArrayLayer			= baseArrayLayer;
			subResRange.layerCount				= layerCount;

			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_1D;

			if (useCubeView)
				viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			else
			{
				if (desc.type == TextureType::Texture1D)
					viewType = subResRange.layerCount > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
				if (desc.type == TextureType::Texture2D)
					viewType = subResRange.layerCount > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
				if (desc.type == TextureType::Texture3D)
					viewType = VK_IMAGE_VIEW_TYPE_3D;
			}

			VkImageViewCreateInfo viewInfo = VkImageViewCreateInfo{};
			viewInfo.sType				   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.pNext				   = nullptr;
			viewInfo.image				   = m_image;
			viewInfo.viewType			   = viewType;
			viewInfo.format				   = VulkanUtility::GetVkFormat(desc.format);
			viewInfo.subresourceRange	   = subResRange;

			const VkResult result = vkCreateImageView(m_device, &viewInfo, nullptr, &imgView);
			SFG_ASSERT(result == VK_SUCCESS);
			VK_NAME_OBJECT(imgView, VK_OBJECT_TYPE_IMAGE_VIEW, desc.name, name);
		};

		m_imageViews = DynamicArray<VkImageView>(desc.viewCount);

		for (uint32 i = 0; i < desc.viewCount; i++)
		{
			TextureView& view = desc.views[i];

			const uint32 baseLevel		= view.baseArrayLevel;
			const uint32 remainingLevel = view.arrayLevels == 0 ? (desc.arrayLevels - baseLevel) : view.arrayLevels;
			const uint32 baseMip		= view.baseMipLevel;
			const uint32 remainingMip	= view.mipLevels == 0 ? (desc.mipLevels - baseMip) : view.mipLevels;
			createView(view.isCubemap, baseLevel, remainingLevel, baseMip, remainingMip, m_imageViews[i]);
		}
	}

	void VulkanTexture::Destroy()
	{
		for (VkImageView view : m_imageViews)
			vkDestroyImageView(m_device, view, nullptr);

		vmaDestroyImage(m_vma, m_image, m_alloc);
		m_image = nullptr;
		m_alloc = nullptr;
	}
} // namespace SFG

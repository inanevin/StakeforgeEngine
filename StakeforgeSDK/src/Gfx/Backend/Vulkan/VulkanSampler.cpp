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

#include "SFG/Gfx/Backend/Vulkan/VulkanSampler.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanUtility.hpp"
#include "SFG/Gfx/Backend/Vulkan/VulkanFunctions.hpp"
#include "SFG/Gfx/Common/SamplerDesc.hpp"
#include "SFG/IO/Assert.hpp"
#include <vulkan/vulkan.h>

namespace SFG
{
	void VulkanSampler::Create(const SamplerDesc& desc)
	{
		VkSamplerCreateInfo info = VkSamplerCreateInfo{};
		info.sType				 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext				 = nullptr;
		info.magFilter			 = VulkanUtility::GetVkFilter(desc.magFilter);
		info.minFilter			 = VulkanUtility::GetVkFilter(desc.minFilter);
		info.mipmapMode			 = VulkanUtility::GetVkSamplerMipmapMode(desc.mipmapMode);
		info.addressModeU		 = VulkanUtility::GetVkSamplerAddressMode(desc.mode);
		info.addressModeV		 = VulkanUtility::GetVkSamplerAddressMode(desc.mode);
		info.addressModeW		 = VulkanUtility::GetVkSamplerAddressMode(desc.mode);
		info.mipLodBias			 = desc.mipLodBias;
		info.anisotropyEnable	 = m_supportsAnisotropy && desc.anisotropy != 0;
		info.maxAnisotropy		 = static_cast<float>(desc.anisotropy);
		info.minLod				 = desc.minLod;
		info.maxLod				 = desc.maxLod;
		info.borderColor		 = VulkanUtility::GetVkBorderColor(desc.borderColor);

		const VkDevice device = static_cast<VkDevice>(m_device);
		VkResult	   res	  = vkCreateSampler(device, &info, nullptr, &m_sampler);
		SFG_ASSERT(res == VK_SUCCESS);

		VK_NAME_OBJECT(m_sampler, VK_OBJECT_TYPE_SAMPLER, desc.debugName, nameInfo);
	}

	void VulkanSampler::Destroy()
	{
		vkDestroySampler(m_device, m_sampler, nullptr);
		m_sampler = nullptr;
	}
} // namespace SFG

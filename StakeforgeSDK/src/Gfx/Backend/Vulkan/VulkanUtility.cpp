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

#include "SFG/Gfx/Backend/Vulkan/VulkanUtility.hpp"
#include "SFG/Gfx/Common/SamplerDesc.hpp"
#include "SFG/Gfx/Common/TextureFormat.hpp"
#include "SFG/IO/Assert.hpp"
#include <vulkan/vulkan.h>

namespace SFG
{
	VkFormat VulkanUtility::GetVkFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::UNDEFINED:
			return VK_FORMAT_UNDEFINED;
			// 8 bit
		case TextureFormat::R8_SINT:
			return VK_FORMAT_R8_SINT;
		case TextureFormat::R8_UINT:
			return VK_FORMAT_R8_UINT;
		case TextureFormat::R8_UNORM:
			return VK_FORMAT_R8_UNORM;
		case TextureFormat::R8_SNORM:
			return VK_FORMAT_R8_SNORM;

		case TextureFormat::R8G8_SINT:
			return VK_FORMAT_R8G8_SINT;
		case TextureFormat::R8G8_UINT:
			return VK_FORMAT_R8G8_UINT;
		case TextureFormat::R8G8_UNORM:
			return VK_FORMAT_R8G8_UNORM;
		case TextureFormat::R8G8_SNORM:
			return VK_FORMAT_R8G8_SNORM;

		case TextureFormat::R8G8B8A8_SINT:
			return VK_FORMAT_R8G8B8A8_SINT;
		case TextureFormat::R8G8B8A8_UINT:
			return VK_FORMAT_R8G8B8A8_UINT;
		case TextureFormat::R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::R8G8B8A8_SNORM:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case TextureFormat::R8G8B8A8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;

		case TextureFormat::B8G8R8A8_UNORM:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case TextureFormat::B8G8R8A8_SRGB:
			return VK_FORMAT_B8G8R8A8_SRGB;

			// 16 bit
		case TextureFormat::R16_SINT:
			return VK_FORMAT_R16_SINT;
		case TextureFormat::R16_UINT:
			return VK_FORMAT_R16_UINT;
		case TextureFormat::R16_UNORM:
			return VK_FORMAT_R16_UNORM;
		case TextureFormat::R16_SNORM:
			return VK_FORMAT_R16_SNORM;
		case TextureFormat::R16_SFLOAT:
			return VK_FORMAT_R16_SFLOAT;

		case TextureFormat::R16G16_SINT:
			return VK_FORMAT_R16G16_SINT;
		case TextureFormat::R16G16_UINT:
			return VK_FORMAT_R16G16_UINT;
		case TextureFormat::R16G16_UNORM:
			return VK_FORMAT_R16G16_UNORM;
		case TextureFormat::R16G16_SNORM:
			return VK_FORMAT_R16G16_SNORM;
		case TextureFormat::R16G16_SFLOAT:
			return VK_FORMAT_R16G16_SFLOAT;

		case TextureFormat::R16G16B16A16_SINT:
			return VK_FORMAT_R16G16B16A16_SINT;
		case TextureFormat::R16G16B16A16_UINT:
			return VK_FORMAT_R16G16B16A16_UINT;
		case TextureFormat::R16G16B16A16_UNORM:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case TextureFormat::R16G16B16A16_SNORM:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case TextureFormat::R16G16B16A16_SFLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;

			// 32 bit
		case TextureFormat::R32_SINT:
			return VK_FORMAT_R32_SINT;
		case TextureFormat::R32_UINT:
			return VK_FORMAT_R32_UINT;
		case TextureFormat::R32_SFLOAT:
			return VK_FORMAT_R32_SFLOAT;

		case TextureFormat::R32G32_SINT:
			return VK_FORMAT_R32G32_SINT;
		case TextureFormat::R32G32_UINT:
			return VK_FORMAT_R32G32_UINT;
		case TextureFormat::R32G32_SFLOAT:
			return VK_FORMAT_R32G32_SFLOAT;

		case TextureFormat::R32G32B32_SFLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case TextureFormat::R32G32B32_SINT:
			return VK_FORMAT_R32G32B32_SINT;
		case TextureFormat::R32G32B32_UINT:
			return VK_FORMAT_R32G32B32_UINT;

		case TextureFormat::R32G32B32A32_SINT:
			return VK_FORMAT_R32G32B32A32_SINT;
		case TextureFormat::R32G32B32A32_UINT:
			return VK_FORMAT_R32G32B32A32_UINT;
		case TextureFormat::R32G32B32A32_SFLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

			// depth-stencil
		case TextureFormat::D32_SFLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case TextureFormat::D24_UNORM_S8_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case TextureFormat::D16_UNORM:
			return VK_FORMAT_D16_UNORM;

			// misc
		case TextureFormat::R11G11B10_SFLOAT:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case TextureFormat::R10G0B10A2_INT:
			return VK_FORMAT_A2B10G10R10_SINT_PACK32;
		case TextureFormat::BC3_BLOCK_SRGB:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case TextureFormat::BC3_BLOCK_UNORM:
			return VK_FORMAT_BC3_UNORM_BLOCK;

		default:
			return VK_FORMAT_UNDEFINED;
		}
	}
	VkFilter VulkanUtility::GetVkFilter(SamplerFilter filter)
	{
		switch (filter)
		{
		case SamplerFilter::Linear:
			return VK_FILTER_LINEAR;
		case SamplerFilter::Nearest:
			return VK_FILTER_NEAREST;
		default:
			return VK_FILTER_LINEAR;
		}
	}
	VkSamplerAddressMode VulkanUtility::GetVkSamplerAddressMode(SamplerAddressMode mode)
	{
		switch (mode)
		{
		case SamplerAddressMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case SamplerAddressMode::MirroredRepeat:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case SamplerAddressMode::ClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case SamplerAddressMode::ClampToBorder:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case SamplerAddressMode::MirrorClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}
	VkSamplerMipmapMode VulkanUtility::GetVkSamplerMipmapMode(SamplerMipmapMode mode)
	{
		switch (mode)
		{
		case SamplerMipmapMode::Nearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SamplerMipmapMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		default:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}
	} // namespace SFG

	VkBorderColor VulkanUtility::GetVkBorderColor(SamplerBorderColor color)
	{
		switch (color)
		{
		case SamplerBorderColor::BlackTransparent:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		case SamplerBorderColor::BlackOpaque:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		case SamplerBorderColor::WhiteOpaque:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		default:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
	}

	VkSampleCountFlagBits VulkanUtility::GetVkSamples(unsigned int samples)
	{
		if (samples == 1)
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

		if (samples == 2)
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;

		if (samples == 4)
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;

		if (samples == 8)
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;

		SFG_ASSERT(false);
		return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	}
} // namespace SFG
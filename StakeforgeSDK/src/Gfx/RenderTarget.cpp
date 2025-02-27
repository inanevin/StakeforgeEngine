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

#include "SFG/Gfx/RenderTarget.hpp"
#include "SFG/Gfx/Common/RenderTargetDesc.hpp"
#include "SFG/Gfx/Common/TextureDesc.hpp"
#include "SFG/Gfx/Common/TextureFlags.hpp"
#include "SFG/Gfx/GfxResources.hpp"

namespace SFG
{
	void RenderTarget::Create(const RenderTargetDesc& desc)
	{
		if (!desc.isSwapchain)
		{
			m_isSwapchain = false;

			TextureView view = {
				.baseArrayLevel = 0,
				.arrayLevels	= 0,
				.baseMipLevel	= 0,
				.mipLevels		= 0,
				.isCubemap		= false,
			};

			for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				m_textures[i] = m_gfxResources.CreateTexture({
					.name	= desc.name,
					.format = desc.format,
					.views	= &view,
					.width	= desc.width,
					.height = desc.height,
					.flags	= desc.isDepth ? TextureFlags::TEXTURE_FLAGS_DEPTH_ATT : TextureFlags::TEXTURE_FLAGS_COLOR_ATT,
				});
			}

			return;
		}

		m_textures[0] = desc.swapchainHandle;
		m_isSwapchain = true;
	}

	void RenderTarget::Destroy()
	{
		if (!m_isSwapchain)
		{
			for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				m_gfxResources.DestroyTexture(m_textures[i]);
			}
		}
	}

} // namespace SFG

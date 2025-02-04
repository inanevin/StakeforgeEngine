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

#include "SFG/Resources/Texture.hpp"
#include "SFG/Resources/TextureLoadOptions.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Memory/Memory.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "SFG/Vendor/stb/stb_image.h"

namespace SFG
{
	bool Texture::LoadFromFile(const char* file, const TextureLoadOptions& options)
	{
		DestroyBuffers();

		int		   x = 0, y = 0, comps = 0;
		const bool is16 = stbi_is_16_bit(file);

		uint8  bitDepth = 0;
		uint8  channels = 0;
		uint8* data		= nullptr;

		if (is16 && !options.force8Bit)
		{
			stbi_us* pixels = stbi_load_16(file, &x, &y, &comps, options.desiredChannels);

			if (!pixels)
			{
				SFG_ERR("Failed loading texture: {0}", file);
				return false;
			}

			bitDepth = 16;
			channels = (options.desiredChannels == 0 ? static_cast<uint8>(comps) : options.desiredChannels);
			data	 = reinterpret_cast<uint8*>(pixels);
		}
		else
		{
			unsigned char* pixels = stbi_load(file, &x, &y, &comps, options.desiredChannels);

			if (!pixels)
			{
				SFG_ERR("Failed loading texture: {0}", file);
				return false;
			}

			bitDepth = 8;
			channels = (options.desiredChannels == 0 ? static_cast<uint8>(comps) : options.desiredChannels);
			data	 = reinterpret_cast<uint8*>(pixels);
		}

		const TextureBuffer buffer = {
			.data	  = (uint8*)data,
			.width	  = static_cast<uint16>(x),
			.height	  = static_cast<uint16>(y),
			.channels = channels,
			.bitDepth = bitDepth,
		};
		m_buffers.push_back(buffer);
		return true;
	}

	void Texture::LoadFromStream(IStream& stream)
	{
	}

	void Texture::DestroyBuffers()
	{
		for (const TextureBuffer& buffer : m_buffers)
			FREE(buffer.data);
		m_buffers.resize(0);
	}
} // namespace SFG

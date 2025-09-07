// Copyright (c) 2025 Inan Evin
#include "image_util.hpp"
#include "io/log.hpp"
#include "math/vector2ui16.hpp"
#include "math/math.hpp"
#include "gfx/common/gfx_common.hpp"
#include "memory/memory.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "vendor/stb/stb_image.h"
#include "vendor/stb/stb_image_resize.h"

namespace SFG
{
	void* image_util::load_from_file_ch(const char* file, uint8 force_channels)
	{
		int		 x = 0, y = 0, comp = 0;
		stbi_uc* data = stbi_load(file, &x, &y, &comp, static_cast<int>(force_channels));

		if (data == nullptr)
		{
			SFG_ERR("Failed loading image from file! {0}", file);
			return nullptr;
		}

		return data;
	}

	void* image_util::load_from_file_ch(const char* file, vector2ui16& out_size, uint8 force_channels)
	{
		int		 x = 0, y = 0, comp = 0;
		stbi_uc* data = stbi_load(file, &x, &y, &comp, static_cast<int>(force_channels));

		if (data == nullptr)
		{
			SFG_ERR("Failed loading image from file! {0}", file);
			return nullptr;
		}

		out_size = vector2ui16(static_cast<uint16>(x), static_cast<uint16>(y));

		return data;
	}

	void* image_util::load_from_file(const char* file, uint8& out_channels)
	{
		int		 x = 0, y = 0, comp = 0;
		stbi_uc* data = stbi_load(file, &x, &y, &comp, 0);

		if (data == nullptr)
		{
			SFG_ERR("Failed loading image from file! {0}", file);
			return nullptr;
		}

		out_channels = static_cast<int>(comp);

		return data;
	}

	void* image_util::load_from_file(const char* file, vector2ui16& out_size, uint8& out_channels)
	{
		int		 x = 0, y = 0, comp = 0;
		stbi_uc* data = stbi_load(file, &x, &y, &comp, 0);

		if (data == nullptr)
		{
			SFG_ERR("Failed loading image from file! {0}", file);
			return nullptr;
		}

		out_channels = static_cast<int>(comp);
		out_size	 = vector2ui16(static_cast<uint16>(x), static_cast<uint16>(y));

		return data;
	}

	void image_util::generate_mips(texture_buffer* out_buffers, uint8 target_levels, mip_gen_filter filter, uint8 channels, bool is_linear, bool premultiplied_alpha)
	{
		const texture_buffer& buf		  = out_buffers[0];
		uint8*				  last_pixels = buf.pixels;
		uint16				  last_w	  = buf.size.x;
		uint16				  last_h	  = buf.size.y;

		for (uint8 i = 0; i < target_levels - 1; i++)
		{
			uint16 w = last_w / 2;
			uint16 h = last_h / 2;

			if (w < 1)
				w = 1;

			if (h < 1)
				h = 1;

			texture_buffer mip		  = {};
			mip.size				  = vector2ui16(w, h);
			mip.pixels				  = (uint8*)SFG_MALLOC(w * h * buf.bpp);
			mip.bpp					  = buf.bpp;
			const stbir_colorspace cs = is_linear ? stbir_colorspace::STBIR_COLORSPACE_LINEAR : stbir_colorspace::STBIR_COLORSPACE_SRGB;

			int ret = 0;

			const uint32 alpha_ch = mip.bpp == 1 ? 0 : 3;

			uint32 flags = premultiplied_alpha ? STBIR_FLAG_ALPHA_PREMULTIPLIED : 0;

			if (mip.bpp == 4 || mip.bpp == 1)
				ret = stbir_resize_uint8_generic(last_pixels, last_w, last_h, 0, mip.pixels, w, h, 0, channels, alpha_ch, flags, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter), cs, 0);
			else
				ret = stbir_resize_uint16_generic((uint16*)last_pixels, last_w, last_h, 0, (uint16*)mip.pixels, w, h, 0, channels, alpha_ch, flags, stbir_edge::STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter), cs, 0);

			last_w			   = w;
			last_h			   = h;
			last_pixels		   = mip.pixels;
			out_buffers[i + 1] = mip;
		}
	}

	uint8 image_util::calculate_mip_levels(uint16 width, uint16 height)
	{
		return static_cast<uint8>(math::floor_log2(math::max(width, height))) + 1;
	}

	void image_util::free(void* data)
	{
		STBI_FREE(data);
	}

}
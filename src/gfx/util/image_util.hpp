// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
namespace SFG
{
	struct vector2ui16;
	struct texture_buffer;

	class image_util
	{
	public:
		enum class mip_gen_filter
		{
			def = 0,
			box,
			triangle,
			cubic_spline,
			catmullrom,
			mitchell,
		};
		static void* load_from_file_ch(const char* file, uint8 force_channels);
		static void* load_from_file_ch(const char* file, vector2ui16& out_size, uint8 force_channels);
		static void* load_from_file(const char* file, uint8& out_channels);
		static void* load_from_file(const char* file, vector2ui16& out_size, uint8& out_channels);
		static void	 generate_mips(texture_buffer* out_buffers, uint8 targetx_levels, mip_gen_filter filter, uint8 channels, bool is_linear, bool premultiplied_alpha);
		static uint8 calculate_mip_levels(uint16 width, uint16 height);
		static void	 free(void* data);
	};
}
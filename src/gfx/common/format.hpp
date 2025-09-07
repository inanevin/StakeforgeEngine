// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

namespace SFG
{
	enum class format : uint8
	{
		undefined = 0,

		// 8 bit
		r8_sint,
		r8_uint,
		r8_unorm,
		r8_snorm,

		r8g8_sint,
		r8g8_uint,
		r8g8_unorm,
		r8g8_snorm,

		r8g8b8a8_sint,
		r8g8b8a8_uint,
		r8g8b8a8_unorm,
		r8g8b8a8_snorm,
		r8g8b8a8_srgb,

		b8g8r8a8_unorm,
		b8g8r8a8_srgb,

		// 16 bit
		r16_sint,
		r16_uint,
		r16_unorm,
		r16_snorm,
		r16_sfloat,

		r16g16_sint,
		r16g16_uint,
		r16g16_unorm,
		r16g16_snorm,
		r16g16_sfloat,

		r16g16b16a16_sint,
		r16g16b16a16_uint,
		r16g16b16a16_unorm,
		r16g16b16a16_snorm,
		r16g16b16a16_sfloat,

		// 32 bit
		r32_sint,
		r32_uint,
		r32_sfloat,

		r32g32_sint,
		r32g32_uint,
		r32g32_sfloat,

		r32g32b32_sfloat,
		r32g32b32_sint,
		r32g32b32_uint,

		r32g32b32a32_sint,
		r32g32b32a32_uint,
		r32g32b32a32_sfloat,

		// depth-stencil
		d32_sfloat,
		d24_unorm_s8_uint,
		d16_unorm,

		// misc
		r11g11b10_sfloat,
		r10g0b10a2_int,
		bc3_block_srgb,
		bc3_block_unorm,
		format_max,
	};

	extern uint8 format_get_bpp(format fmt);
	extern uint8 format_get_channels(format fmt);
	extern bool	 format_is_linear(format fmt);

#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const format& f);
	void from_json(const nlohmann::json& j, format& f);
#endif
}
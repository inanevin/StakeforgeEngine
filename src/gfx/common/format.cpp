// Copyright (c) 2025 Inan Evin
#include "format.hpp"
#include "io/assert.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
	uint8 format_get_bpp(format fmt)
	{
		switch (fmt)
		{
		case format::r8_unorm:
			return 1;
		case format::r8g8b8a8_unorm:
		case format::r8g8b8a8_srgb:
			return 4;
		default:
			break;
		}

		SFG_ASSERT(false);
		return 0;
	}

	uint8 format_get_channels(format fmt)
	{
		switch (fmt)
		{
		case format::r8_unorm:
			return 1;
		case format::r8g8b8a8_unorm:
		case format::r8g8b8a8_srgb:
			return 4;
		default:
			break;
		}
		SFG_ASSERT(false);
		return 0;
	}

	bool format_is_linear(format fmt)
	{
		switch (fmt)
		{
		case format::b8g8r8a8_srgb:
		case format::bc3_block_srgb:
		case format::r8g8b8a8_srgb:
			return false;
		default:
			return true;
		}

		SFG_ASSERT(false);
		return true;
	}

#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const format& f)
	{
		switch (f)
		{
		case format::r8_unorm:
			j = "r8";
			return;
		case format::r8g8b8a8_srgb:
			j = "r8g8b8a8_srgb";
			return;
		case format::r8g8b8a8_unorm:
			j = "r8g8b8a8_unorm";
			return;
		case format::r32g32_sfloat:
			j = "r32g32_sfloat";
			return;
		case format::r32g32b32_sfloat:
			j = "r32g32b32_sfloat";
			return;
		case format::r32g32b32a32_sfloat:
			j = "r32g32b32a32_sfloat";
			return;
		case format::r32g32b32a32_uint:
			j = "r32g32b32a32_uint";
			return;
		}

		j = "undefined";
	}

	void from_json(const nlohmann::json& j, format& f)
	{
		const string str = j.get<string>();

		if (str.compare("r8") == 0)
		{
			f = format::r8_unorm;
			return;
		}
		if (str.compare("r8g8b8a8_srgb") == 0)
		{
			f = format::r8g8b8a8_srgb;
			return;
		}
		if (str.compare("r8g8b8a8_unorm") == 0)
		{
			f = format::r8g8b8a8_unorm;
			return;
		}
		if (str.compare("r32g32_sfloat") == 0)
		{
			f = format::r32g32_sfloat;
			return;
		}
		if (str.compare("r32g32b32_sfloat") == 0)
		{
			f = format::r32g32b32_sfloat;
			return;
		}
		if (str.compare("r32g32b32a32_sfloat") == 0)
		{
			f = format::r32g32b32a32_sfloat;
			return;
		}
		if (str.compare("r32g32b32a32_uint") == 0)
		{
			f = format::r32g32b32a32_uint;
			return;
		}

		f = format::undefined;
	}
#endif
}
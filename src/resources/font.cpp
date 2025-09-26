// Copyright (c) 2025 Inan Evin

#include "font.hpp"
#include "font_raw.hpp"
#include "gui/vekt.hpp"

namespace SFG
{
	void font::create_from_raw(const font_raw& raw, vekt::font_manager& fm)
	{
		// ffs man.
		unsigned char* data = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(raw.font_data.data()));
		_font				= fm.load_font(data, static_cast<unsigned int>(raw.font_data.size()), static_cast<unsigned int>(raw.point_size), 32, 128, static_cast<vekt::font_type>(raw.font_type), raw.sdf_padding, raw.sdf_edge, raw.sdf_distance);
	}
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/string.hpp"

namespace SFG
{
	class color;

	class color_utils
	{

	public:
		static color  lerp(const color& c1, const color& c2, float a);
		static color  from_hex(const string& hex);
		static string to_hex(const color& color);
		static color  hs_to_srgb(const color& color);
		static color  srgb_to_hsv(const color& color);
		static color  hsv_to_srgb(const color& color);
		static color  srgb_to_linear(const color& color);
		static color  linear_to_srgb(const color& color);
		static color  brighten(const color& color, float amt);
		static color  darken(const color& color, float amt);
	};

} // namespace SFG

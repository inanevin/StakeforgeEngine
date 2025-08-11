// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "color_utils.hpp"
#include "color.hpp"
#include "math.hpp"
#include "easing.hpp"
#include <iomanip>
#include <sstream>

namespace SFG
{
	color color_utils::lerp(const color& c1, const color& c2, float a)
	{
		return color(easing::lerp(c1.x, c2.x, a), easing::lerp(c1.y, c2.y, a), easing::lerp(c1.z, c2.z, a), easing::lerp(c1.w, c2.w, a));
	}

	color color_utils::from_hex(const string& hex)
	{
		if (hex.size() != 7)
			return color::black;

		if (hex[0] != '#')
			return color::black;

		uint32 r, g, b;

#ifdef SFG_PLATFORM_WINDOWS
		sscanf_s(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#else
		const int ret = std::sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#endif
		return color{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f};
	}

	string color_utils::to_hex(const color& color)
	{
		const int32		  r = static_cast<int32>(color.x * 255);
		const int32		  g = static_cast<int32>(color.y * 255);
		const int32		  b = static_cast<int32>(color.z * 255);
		std::stringstream ss;
		ss << "#" << std::hex << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
		return ss.str();
	}

	color color_utils::hs_to_srgb(const color& col)
	{
		const float hue		   = col.x;
		const float saturation = col.y;
		const float angle	   = hue * 6.0f;
		const float r		   = math::clamp(math::abs(angle - 3.0f) - 1.0f, 0.0f, 1.0f);
		const float g		   = math::clamp(2.0f - math::abs(angle - 2.0f), 0.0f, 1.0f);
		const float b		   = math::clamp(2.0f - math::abs(angle - 4.0f), 0.0f, 1.0f);
		return lerp(color::white, color(r, g, b, 1.0f), saturation);
	}

	color color_utils::srgb_to_hsv(const color& col)
	{
		color hsv;

		float minVal = math::min(col.x, math::min(col.y, col.z));
		float maxVal = math::max(col.x, math::max(col.y, col.z));
		float delta	 = maxVal - minVal;

		// Hue calculation
		if (delta == 0)
		{
			hsv.x = 0;
		}
		else if (maxVal == col.x)
		{
			float integ = 0.0f;
			hsv.x		= 60 * math::modf(((col.y - col.z) / delta), &integ);
		}
		else if (maxVal == col.y)
		{
			hsv.x = 60 * (((col.z - col.x) / delta) + 2);
		}
		else if (maxVal == col.z)
		{
			hsv.x = 60 * (((col.x - col.y) / delta) + 4);
		}

		if (hsv.x < 0)
		{
			hsv.x += 360;
		}

		// Saturation calculation
		hsv.y = (maxVal == 0) ? 0 : (delta / maxVal);

		// Value calculation
		hsv.z = maxVal;
		hsv.w = col.w;

		return hsv;
	}

	color color_utils::hsv_to_srgb(const color& col)
	{
		color rgb;
		float C		= col.z * col.y;
		float integ = 0.0f;
		float X		= C * (1 - math::abs(math::modf(col.x / 60.0f, &integ) - 1.0f));
		float m		= col.z - C;
		float R1, G1, B1;

		if (col.x >= 0 && col.x < 60)
		{
			R1 = C;
			G1 = X;
			B1 = 0;
		}
		else if (col.x >= 60 && col.x < 120)
		{
			R1 = X;
			G1 = C;
			B1 = 0;
		}
		else if (col.x >= 120 && col.x < 180)
		{
			R1 = 0;
			G1 = C;
			B1 = X;
		}
		else if (col.x >= 180 && col.x < 240)
		{
			R1 = 0;
			G1 = X;
			B1 = C;
		}
		else if (col.x >= 240 && col.x < 300)
		{
			R1 = X;
			G1 = 0;
			B1 = C;
		}
		else
		{
			R1 = C;
			G1 = 0;
			B1 = X;
		}

		rgb.x = R1 + m;
		rgb.y = G1 + m;
		rgb.z = B1 + m;
		rgb.w = col.w;

		return rgb;
	}

	color color_utils::srgb_to_linear(const color& col)
	{
		auto convert = [](float value) {
			if (value <= 0.04045f)
			{
				return value / 12.92f;
			}
			else
			{
				return math::pow((value + 0.055f) / 1.055f, 2.4f);
			}
		};

		return color(convert(col.x), convert(col.y), convert(col.z), convert(col.w));
	}

	color color_utils::linear_to_srgb(const color& col)
	{
		auto convert = [](float value) {
			if (value <= 0.0031308f)
			{
				return value * 12.92f;
			}
			else
			{
				return 1.055f * math::pow(value, 1.0f / 2.4f) - 0.055f;
			}
		};

		return color(convert(col.x), convert(col.y), convert(col.z), convert(col.w));
	}

	color color_utils::brighten(const color& color, float amt)
	{
		return lerp(color, color::white, amt);
	}

	color color_utils::darken(const color& color, float amt)
	{
		return lerp(color, color::white, amt);
	}

} // namespace SFG

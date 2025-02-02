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

#include "SFG/Math/ColorUtils.hpp"
#include "SFG/Math/Color.hpp"
#include "SFG/Math/Math.hpp"
#include <iomanip>
#include <sstream>

namespace SFG
{
	Color ColorUtils::FromHex(const String& hex)
	{
		if (hex.size() != 7)
			return Color::Black;

		if (hex[0] != '#')
			return Color::Black;

		uint32 r, g, b;

#ifdef SFG_PLATFORM_WINDOWS
		sscanf_s(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#else
		const int ret = std::sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
#endif
		return Color{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f};
	}

	String ColorUtils::ToHex(const Color& color)
	{
		const int32		  r = static_cast<int32>(color.x * 255);
		const int32		  g = static_cast<int32>(color.y * 255);
		const int32		  b = static_cast<int32>(color.z * 255);
		std::stringstream ss;
		ss << "#" << std::hex << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
		return ss.str();
	}

	Color ColorUtils::HS2SRGB(const Color& color)
	{
		const float hue		   = color.x;
		const float saturation = color.y;
		const float angle	   = hue * 6.0f;
		const float r		   = Math::Clamp(Math::Abs(angle - 3.0f) - 1.0f, 0.0f, 1.0f);
		const float g		   = Math::Clamp(2.0f - Math::Abs(angle - 2.0f), 0.0f, 1.0f);
		const float b		   = Math::Clamp(2.0f - Math::Abs(angle - 4.0f), 0.0f, 1.0f);
		return Math::Lerp(Color::White, Color(r, g, b, 1.0f), saturation);
	}

	Color ColorUtils::SRGB2HSV(const Color& color)
	{
		Color hsv;

		float minVal = Math::Min(color.x, Math::Min(color.y, color.z));
		float maxVal = Math::Max(color.x, Math::Max(color.y, color.z));
		float delta	 = maxVal - minVal;

		// Hue calculation
		if (delta == 0)
		{
			hsv.x = 0;
		}
		else if (maxVal == color.x)
		{
			hsv.x = 60 * Math::Fmod(((color.y - color.z) / delta), 6);
		}
		else if (maxVal == color.y)
		{
			hsv.x = 60 * (((color.z - color.x) / delta) + 2);
		}
		else if (maxVal == color.z)
		{
			hsv.x = 60 * (((color.x - color.y) / delta) + 4);
		}

		if (hsv.x < 0)
		{
			hsv.x += 360;
		}

		// Saturation calculation
		hsv.y = (maxVal == 0) ? 0 : (delta / maxVal);

		// Value calculation
		hsv.z = maxVal;
		hsv.w = color.w;

		return hsv;
	}

	Color ColorUtils::HSV2SRGB(const Color& color)
	{
		Color rgb;
		float C = color.z * color.y;
		float X = C * (1 - Math::Abs(Math::Fmod(color.x / 60.0f, 2.0f) - 1.0f));
		float m = color.z - C;
		float R1, G1, B1;

		if (color.x >= 0 && color.x < 60)
		{
			R1 = C;
			G1 = X;
			B1 = 0;
		}
		else if (color.x >= 60 && color.x < 120)
		{
			R1 = X;
			G1 = C;
			B1 = 0;
		}
		else if (color.x >= 120 && color.x < 180)
		{
			R1 = 0;
			G1 = C;
			B1 = X;
		}
		else if (color.x >= 180 && color.x < 240)
		{
			R1 = 0;
			G1 = X;
			B1 = C;
		}
		else if (color.x >= 240 && color.x < 300)
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
		rgb.w = color.w;

		return rgb;
	}

	Color ColorUtils::SRGB2Linear(const Color& color)
	{
		auto convert = [](float value) {
			if (value <= 0.04045f)
			{
				return value / 12.92f;
			}
			else
			{
				return Math::Pow((value + 0.055f) / 1.055f, 2.4f);
			}
		};

		return Color(convert(color.x), convert(color.y), convert(color.z), convert(color.w));
	}

	Color ColorUtils::Linear2SRGB(const Color& color)
	{
		auto convert = [](float value) {
			if (value <= 0.0031308f)
			{
				return value * 12.92f;
			}
			else
			{
				return 1.055f * Math::Pow(value, 1.0f / 2.4f) - 0.055f;
			}
		};

		return Color(convert(color.x), convert(color.y), convert(color.z), convert(color.w));
	}

	Color ColorUtils::Brighten(const Color& color, float amt)
	{
		return Math::Lerp(color, Color::White, amt);
	}

	Color ColorUtils::Darken(const Color& color, float amt)
	{
		return Math::Lerp(color, Color::White, amt);
	}

} // namespace SFG

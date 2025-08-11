// Copyright (c) 2025 Inan Evin

#include "color.hpp"
#include "math.hpp"
#include "data/istream.hpp"
#include "data/ostream.hpp"

namespace SFG
{
	color color::red	= color(1, 0, 0, 1);
	color color::green	= color(0, 1, 0);
	color color::blue	= color(0, 0, 1);
	color color::cyan	= color(0, 1, 1);
	color color::yellow = color(1, 1, 0);
	color color::black	= color(0, 0, 0);
	color color::white	= color(1, 1, 1);
	color color::purple = color(1, 0, 1);
	color color::maroon = color(0.5f, 0, 0);
	color color::beige	= color(0.96f, 0.96f, 0.862f);
	color color::brown	= color(0.647f, 0.164f, 0.164f);
	color color::gray	= color(0.5f, 0.5f, 0.5f);

	color color::linear_to_srgb(const color& linear_color)
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

		return color(convert(linear_color.x), convert(linear_color.y), convert(linear_color.z), convert(linear_color.w));
	}

	color color::srgb_to_linear(const color& srgb_color)
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

		return color(convert(srgb_color.x), convert(srgb_color.y), convert(srgb_color.z), convert(srgb_color.w));
	}

	color color::from255(float r, float g, float b, float a)
	{
		return color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	void color::round()
	{
		x = math::round(x);
		y = math::round(y);
		z = math::round(z);
		w = math::round(w);
	}

	void color::serialize(ostream& stream) const
	{
		stream << x << y << z << w;
	}

	void color::deserialize(istream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	vector4 color::to_vector() const
	{
		return vector4(x, y, z, w);
	}

} // namespace SFG

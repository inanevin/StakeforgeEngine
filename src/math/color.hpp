// Copyright (c) 2025 Inan Evin

#pragma once
#include "vector4.hpp"

namespace SFG
{
	class ostream;
	class istream;

	class color
	{

	public:
		color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f) : x(rv), y(gv), z(bv), w(av){};
		static color from255(float r, float g, float b, float a);
		static color linear_to_srgb(const color& linear_color);
		static color srgb_to_linear(const color& srgb_color);

		vector4 to_vector() const;
		void	round();
		void	serialize(ostream& stream) const;
		void	deserialize(istream& stream);

		bool operator!=(const color& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		bool operator==(const color& rhs) const
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}

		color operator*(const float& rhs) const
		{
			return color(x * rhs, y * rhs, z * rhs, w * rhs);
		}

		color operator+(const color& rhs) const
		{
			return color(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		color operator/(float v) const
		{
			return color(x / v, y / v, z / v, w / v);
		}

		color operator/=(float v) const
		{
			return color(x / v, y / v, z / v, w / v);
		}

		color operator*=(float v) const
		{
			return color(x * v, y * v, z * v, w * v);
		}

		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}

		static color red;
		static color green;
		static color LightBlue;
		static color blue;
		static color DarkBlue;
		static color cyan;
		static color yellow;
		static color black;
		static color white;
		static color purple;
		static color maroon;
		static color beige;
		static color brown;
		static color gray;

		float x, y, z, w = 1.0f;
	};

} // namespace SFG

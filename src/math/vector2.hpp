// Copyright (c) 2025 Inan Evin
#pragma once

#include "math_common.hpp"

#undef min
#undef max

namespace SFG
{
	class vector2
	{
	public:
		vector2(){};
		vector2(float _x, float _y) : x(_x), y(_y){};

		float x = 0.0f;
		float y = 0.0f;

		static vector2 zero;
		static vector2 one;

		static vector2 clamp(const vector2& vector, const vector2& min, const vector2& max);
		static vector2 clamp_magnitude(const vector2& vector, float max_length);
		static vector2 abs(const vector2& vector);
		static vector2 min(const vector2& a, const vector2& b);
		static vector2 max(const vector2& a, const vector2& b);
		static float   dot(const vector2& a, const vector2& b);
		static float   distance(const vector2& a, const vector2& b);
		static float   angle(const vector2& a, const vector2& b);

		vector2 normalized() const;
		bool	equals(const vector2& other, float epsilon = MATH_EPS) const;
		bool	is_zero(float epsilon = MATH_EPS) const;
		float	magnitude() const;
		float	magnitude_sqr() const;

		inline vector2 operator+(const vector2& other) const
		{
			return vector2(x + other.x, y + other.y);
		}
		inline vector2 operator-(const vector2& other) const
		{
			return vector2(x - other.x, y - other.y);
		}
		inline vector2 operator*(float scalar) const
		{
			return vector2(x * scalar, y * scalar);
		}

		inline vector2 operator/(float scalar) const
		{
			if (scalar == 0.0f)
			{
				return vector2(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
			}
			return vector2(x / scalar, y / scalar);
		}

		inline vector2& operator+=(const vector2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		inline vector2& operator-=(const vector2& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline vector2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		inline vector2& operator/=(float scalar)
		{
			if (scalar == 0.0f)
			{
				x = MATH_INF_F;
				y = MATH_INF_F;
			}
			else
			{
				x /= scalar;
				y /= scalar;
			}
			return *this;
		}

		inline bool operator==(const vector2& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const vector2& other) const
		{
			return !(*this == other);
		}
	};

}
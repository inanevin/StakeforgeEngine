// Copyright (c) 2025 Inan Evin
#pragma once
#include "math_common.hpp"

#undef min
#undef max

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

namespace SFG
{

	class istream;
	class ostream;

	class vector4
	{
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		vector4() = default;
		vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
		{
		}

		static const vector4 zero;
		static const vector4 one;

		static vector4 clamp(const vector4& vector, const vector4& min, const vector4& max);
		static vector4 abs(const vector4& vector);
		static vector4 min(const vector4& a, const vector4& b);
		static vector4 max(const vector4& a, const vector4& b);
		static float   dot(const vector4& a, const vector4& b);
		static float   distance(const vector4& a, const vector4& b);
		vector4		   project(const vector4& on_normal) const;
		vector4		   rotate(const vector4& axis, float angle_degrees) const;
		bool		   equals(const vector4& other, float epsilon = MATH_EPS) const;
		bool		   is_zero(float epsilon = MATH_EPS) const;
		float		   magnitude() const;
		float		   magnitude_sqr() const;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

		inline bool is_point_inside(float _x, float _y) const
		{
			return _x >= x && _x <= x + z && _y >= y && _y <= y + w;
		}

		inline vector4 normalized() const
		{
			float mag = magnitude();
			if (mag > MATH_EPS)
			{
				return vector4(x / mag, y / mag, z / mag, w / mag);
			}
			return vector4::zero;
		}

		inline void normalize()
		{
			float mag = magnitude();
			if (mag > MATH_EPS)
			{
				x /= mag;
				y /= mag;
				z /= mag;
				w /= mag;
			}
			else
			{
				x = y = z = w = 0.0f;
			}
		}

		inline vector4 operator+(const vector4& other) const
		{
			return vector4(x + other.x, y + other.y, z + other.z, w + other.w);
		}
		inline vector4 operator-(const vector4& other) const
		{
			return vector4(x - other.x, y - other.y, z - other.z, w - other.w);
		}
		inline vector4 operator*(float scalar) const
		{
			return vector4(x * scalar, y * scalar, z * scalar, w * scalar);
		}
		vector4 operator/(float scalar) const;

		inline vector4 operator-() const
		{
			return vector4(-x, -y, -z, -w);
		}

		inline vector4& operator+=(const vector4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}
		inline vector4& operator-=(const vector4& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}
		inline vector4& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}
		vector4& operator/=(float scalar);

		inline bool operator==(const vector4& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const vector4& other) const
		{
			return !equals(other);
		}
	};

	inline vector4 operator*(float scalar, const vector4& vector)
	{
		return vector * scalar;
	}

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const vector4& v);
	void from_json(const nlohmann::json& j, vector4& v);

#endif

}
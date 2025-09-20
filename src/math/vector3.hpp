// Copyright (c) 2025 Inan Evin
#pragma once
#include "math_common.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

#undef min
#undef max

namespace SFG
{
	class istream;
	class ostream;

	class vector3
	{
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		vector3() = default;
		vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
		{
		}

		static const vector3 zero;
		static const vector3 one;
		static const vector3 up;
		static const vector3 forward;
		static const vector3 right;

		static vector3 clamp(const vector3& vector, const vector3& min, const vector3& max);
		static vector3 cross(const vector3& a, const vector3& b);
		static vector3 abs(const vector3& vector);
		static vector3 min(const vector3& a, const vector3& b);
		static vector3 max(const vector3& a, const vector3& b);
		static vector3 lerp(const vector3& a, const vector3& b, float t);
		static float   dot(const vector3& a, const vector3& b);
		static float   distance(const vector3& a, const vector3& b);
		static float   distance_sqr(const vector3& a, const vector3& b);
		vector3		   project(const vector3& on_normal) const;
		vector3		   rotate(const vector3& axis, float angle_degrees) const;
		vector3		   reflect(const vector3& in_normal) const;
		bool		   equals(const vector3& other, float epsilon = MATH_EPS) const;
		bool		   is_zero(float epsilon = MATH_EPS) const;
		float		   magnitude() const;
		float		   magnitude_sqr() const;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

		inline vector3 normalized() const
		{
			float mag = magnitude();
			if (mag > MATH_EPS)
			{
				return vector3(x / mag, y / mag, z / mag);
			}
			return vector3::zero;
		}

		inline void normalize()
		{
			float mag = magnitude();
			if (mag > MATH_EPS)
			{
				x /= mag;
				y /= mag;
				z /= mag;
			}
			else
				x = y = z = 0.0f;
		}

		inline vector3 operator+(const vector3& other) const
		{
			return vector3(x + other.x, y + other.y, z + other.z);
		}
		inline vector3 operator-(const vector3& other) const
		{
			return vector3(x - other.x, y - other.y, z - other.z);
		}
		inline vector3 operator*(float scalar) const
		{
			return vector3(x * scalar, y * scalar, z * scalar);
		}
		inline vector3 operator/(float scalar) const
		{
			if (scalar == 0.0f)
				return vector3::zero;
			return vector3(x / scalar, y / scalar, z / scalar);
		}

		inline vector3 operator-() const
		{
			return vector3(-x, -y, -z);
		}

		inline vector3& operator+=(const vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		inline vector3& operator-=(const vector3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		inline vector3& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}
		inline vector3& operator/=(float scalar)
		{
			if (scalar != 0.0f)
			{
				x /= scalar;
				y /= scalar;
				z /= scalar;
			}
			else
			{
				x = MATH_INF_F;
				y = MATH_INF_F;
				z = MATH_INF_F;
			}
			return *this;
		}

		inline bool operator==(const vector3& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const vector3& other) const
		{
			return !equals(other);
		}
	};

	inline vector3 operator*(float scalar, const vector3& vector)
	{
		return vector * scalar;
	}

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const vector3& v);
	void from_json(const nlohmann::json& j, vector3& v);

#endif
}
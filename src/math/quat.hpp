// Copyright (c) 2025 Inan Evin
#pragma once

#include "vector3.hpp"

namespace Game
{
	// LH coordinates
	class quaternion
	{
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		quaternion() = default;
		quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
		{
		}

		static const quaternion identity;

		vector3	   get_right() const;
		vector3	   get_up() const;
		vector3	   get_forward() const;
		quaternion conjugate() const;
		quaternion inverse() const;
		quaternion normalized() const;
		float	   dot(const quaternion& other) const;
		float	   magnitude() const;
		float	   sqr_magnitude() const;
		bool	   equals(const quaternion& other, float epsilon = MATH_EPS) const;

		static quaternion from_euler(float pitch_degrees, float yaw_degrees, float roll_degrees);
		static vector3	  to_euler(const quaternion& q);
		static quaternion angle_axis(float angle_degrees, const vector3& axis);
		static quaternion lerp(const quaternion& a, const quaternion& b, float t);
		static quaternion slerp(const quaternion& a, const quaternion& b, float t);
		static quaternion look_at(const vector3& source_point, const vector3& target_point, const vector3& up_vector);

		inline bool is_identity(float epsilon = MATH_EPS) const
		{
			return equals(identity, epsilon);
		}

		inline quaternion operator+(const quaternion& other) const
		{
			return quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline quaternion operator-(const quaternion& other) const
		{
			return quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		inline quaternion operator*(const quaternion& other) const
		{
			return quaternion(w * other.x + x * other.w + y * other.z - z * other.y, w * other.y + y * other.w + z * other.x - x * other.z, w * other.z + z * other.w + x * other.y - y * other.x, w * other.w - x * other.x - y * other.y - z * other.z);
		}

		inline vector3 operator*(const vector3& v) const
		{
			quaternion p(v.x, v.y, v.z, 0.0f);
			quaternion q_inv	 = this->conjugate();
			quaternion rotated_p = (*this) * p * q_inv;
			return vector3(rotated_p.x, rotated_p.y, rotated_p.z);
		}

		inline quaternion operator*(float scalar) const
		{
			return quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		quaternion operator/(float scalar) const;

		inline quaternion operator-() const
		{
			return quaternion(-x, -y, -z, -w);
		}

		inline quaternion& operator*=(const quaternion& other)
		{
			*this = (*this) * other;
			return *this;
		}

		inline quaternion& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		quaternion& operator/=(float scalar);

		inline bool operator==(const quaternion& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const quaternion& other) const
		{
			return !equals(other);
		}
	};

	inline quaternion operator*(float scalar, const quaternion& q)
	{
		return q * scalar;
	}

}
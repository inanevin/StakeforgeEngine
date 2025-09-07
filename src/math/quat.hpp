// Copyright (c) 2025 Inan Evin
#pragma once

#include "vector3.hpp"

namespace SFG
{
	// LH coordinates
	class quat
	{
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		quat() = default;
		quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
		{
		}

		static const quat identity;

		vector3 get_right() const;
		vector3 get_up() const;
		vector3 get_forward() const;
		quat	conjugate() const;
		quat	inverse() const;
		quat	normalized() const;
		float	dot(const quat& other) const;
		float	magnitude() const;
		float	sqr_magnitude() const;
		bool	equals(const quat& other, float epsilon = MATH_EPS) const;

		static quat	   from_euler(float pitch_degrees, float yaw_degrees, float roll_degrees);
		static vector3 to_euler(const quat& q);
		static quat	   angle_axis(float angle_degrees, const vector3& axis);
		static quat	   lerp(const quat& a, const quat& b, float t);
		static quat	   slerp(const quat& a, const quat& b, float t);
		static quat	   look_at(const vector3& source_point, const vector3& target_point, const vector3& up_vector);

		inline bool is_identity(float epsilon = MATH_EPS) const
		{
			return equals(identity, epsilon);
		}

		inline quat operator+(const quat& other) const
		{
			return quat(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline quat operator-(const quat& other) const
		{
			return quat(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		inline quat operator*(const quat& other) const
		{
			return quat(w * other.x + x * other.w + y * other.z - z * other.y, w * other.y + y * other.w + z * other.x - x * other.z, w * other.z + z * other.w + x * other.y - y * other.x, w * other.w - x * other.x - y * other.y - z * other.z);
		}

		inline vector3 operator*(const vector3& v) const
		{
			quat p(v.x, v.y, v.z, 0.0f);
			quat q_inv	   = this->conjugate();
			quat rotated_p = (*this) * p * q_inv;
			return vector3(rotated_p.x, rotated_p.y, rotated_p.z);
		}

		inline quat operator*(float scalar) const
		{
			return quat(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		quat operator/(float scalar) const;

		inline quat operator-() const
		{
			return quat(-x, -y, -z, -w);
		}

		inline quat& operator*=(const quat& other)
		{
			*this = (*this) * other;
			return *this;
		}

		inline quat& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}

		quat& operator/=(float scalar);

		inline bool operator==(const quat& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const quat& other) const
		{
			return !equals(other);
		}
	};

	inline quat operator*(float scalar, const quat& q)
	{
		return q * scalar;
	}

}
// Copyright (c) 2025 Inan Evin
#include "quat.hpp"
#include "math.hpp"

namespace SFG
{
	const quat quat::identity(0.0f, 0.0f, 0.0f, 1.0f);

	vector3 quat::get_right() const
	{
		return (*this) * vector3(1.0f, 0.0f, 0.0f);
	}

	vector3 quat::get_up() const
	{
		return (*this) * vector3(0.0f, 1.0f, 0.0f);
	}

	vector3 quat::get_forward() const
	{
		return (*this) * vector3(0.0f, 0.0f, 1.0f);
	}

	quat quat::conjugate() const
	{
		return quat(-x, -y, -z, w);
	}

	quat quat::inverse() const
	{
		float mag_sqr = sqr_magnitude();
		if (math::abs(mag_sqr) < MATH_EPS)
			return identity;
		return conjugate() / mag_sqr;
	}

	quat quat::normalized() const
	{
		float mag = magnitude();
		if (math::abs(mag) < MATH_EPS)
			return identity;
		return (*this) / mag;
	}

	float quat::dot(const quat& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float quat::magnitude() const
	{
		return std::sqrt(sqr_magnitude());
	}

	float quat::sqr_magnitude() const
	{
		return x * x + y * y + z * z + w * w;
	}

	quat quat::from_euler(float pitch_degrees, float yaw_degrees, float roll_degrees)
	{
		float pitch_rad = math::degrees_to_radians(pitch_degrees);
		float yaw_rad	= math::degrees_to_radians(yaw_degrees);
		float roll_rad	= math::degrees_to_radians(roll_degrees);

		float cy = math::cos(yaw_rad * 0.5f);
		float sy = math::sin(yaw_rad * 0.5f);
		float cp = math::cos(pitch_rad * 0.5f);
		float sp = math::sin(pitch_rad * 0.5f);
		float cr = math::cos(roll_rad * 0.5f);
		float sr = math::sin(roll_rad * 0.5f);

		quat q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * sp * cr + sy * cp * sr;
		q.y = sy * cp * cr - cy * sp * sr;
		q.z = cy * cp * sr - sy * sp * cr;

		return q;
	}

	vector3 quat::to_euler(const quat& q)
	{
		vector3 euler_angles;

		float sinp	   = 2.0f * (q.y * q.z + q.x * q.w);
		float cosp	   = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		euler_angles.x = math::radians_to_degrees(std::atan2(sinp, cosp));

		float siny	   = 2.0f * (q.x * q.z + q.y * q.w);
		float cosy	   = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		euler_angles.y = math::radians_to_degrees(std::atan2(siny, cosy));

		float sinr	   = 2.0f * (q.x * q.y + q.z * q.w);
		float cosr	   = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		euler_angles.z = math::radians_to_degrees(std::atan2(sinr, cosr));

		if (math::abs(siny) >= 0.999f)
		{
			euler_angles.y = math::radians_to_degrees(std::atan2(2.0f * q.x * q.w - 2.0f * q.y * q.z, 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z));
			euler_angles.z = 0.0f;
		}

		return euler_angles;
	}

	quat quat::angle_axis(float angle_degrees, const vector3& axis)
	{
		float	angle_rad_half	= math::degrees_to_radians(angle_degrees * 0.5f);
		float	s				= math::sin(angle_rad_half);
		vector3 normalized_axis = axis.normalized();
		return quat(normalized_axis.x * s, normalized_axis.y * s, normalized_axis.z * s, math::cos(angle_rad_half));
	}

	quat quat::lerp(const quat& a, const quat& b, float t)
	{
		float dot_product = a.dot(b);
		quat  result	  = b;

		if (dot_product < 0.0f)
		{
			result.x = -b.x;
			result.y = -b.y;
			result.z = -b.z;
			result.w = -b.w;
		}

		return (a * (1.0f - t) + (result * t)).normalized();
	}

	quat quat::slerp(const quat& a, const quat& b, float t)
	{
		float dot_product = a.dot(b);
		quat  b_adjusted  = b;

		if (dot_product < 0.0f)
		{
			dot_product	 = -dot_product;
			b_adjusted.x = -b.x;
			b_adjusted.y = -b.y;
			b_adjusted.z = -b.z;
			b_adjusted.w = -b.w;
		}

		if (dot_product > 0.9995f)
		{
			return lerp(a, b_adjusted, t);
		}

		float theta		= std::acos(dot_product);
		float sin_theta = math::sin(theta);

		float s0 = math::sin((1.0f - t) * theta) / sin_theta;
		float s1 = math::sin(t * theta) / sin_theta;

		return (a * s0) + (b_adjusted * s1);
	}

	quat quat::look_at(const vector3& source_point, const vector3& target_point, const vector3& up_vector)
	{
		vector3 forward_vec	 = (target_point - source_point).normalized();
		vector3 right_vec	 = vector3::cross(up_vector, forward_vec).normalized();
		vector3 final_up_vec = vector3::cross(forward_vec, right_vec);

		float m00 = right_vec.x;
		float m01 = final_up_vec.x;
		float m02 = forward_vec.x;
		float m10 = right_vec.y;
		float m11 = final_up_vec.y;
		float m12 = forward_vec.y;
		float m20 = right_vec.z;
		float m21 = final_up_vec.z;
		float m22 = forward_vec.z;

		quat  q;
		float trace = m00 + m11 + m22;

		if (trace > 0.0f)
		{
			float s = std::sqrt(trace + 1.0f) * 2.0f;
			q.w		= 0.25f * s;
			q.x		= (m21 - m12) / s;
			q.y		= (m02 - m20) / s;
			q.z		= (m10 - m01) / s;
		}
		else if ((m00 > m11) && (m00 > m22))
		{
			float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
			q.w		= (m21 - m12) / s;
			q.x		= 0.25f * s;
			q.y		= (m01 + m10) / s;
			q.z		= (m02 + m20) / s;
		}
		else if (m11 > m22)
		{
			float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
			q.w		= (m02 - m20) / s;
			q.x		= (m01 + m10) / s;
			q.y		= 0.25f * s;
			q.z		= (m12 + m21) / s;
		}
		else
		{
			float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f;
			q.w		= (m10 - m01) / s;
			q.x		= (m02 + m20) / s;
			q.y		= (m12 + m21) / s;
			q.z		= 0.25f * s;
		}

		return q.normalized();
	}

	quat quat::operator/(float scalar) const
	{
		if (math::abs(scalar) < MATH_EPS)
			return identity;
		return quat(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	quat& quat::operator/=(float scalar)
	{
		if (math::abs(scalar) > MATH_EPS)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
		}
		else
		{
			x = y = z = w = MATH_NAN;
		}
		return *this;
	}

	bool quat::equals(const quat& other, float epsilon) const
	{
		return math::almost_equal(x, other.x, epsilon) && math::almost_equal(y, other.y, epsilon) && math::almost_equal(z, other.z, epsilon) && math::almost_equal(w, other.w, epsilon);
	}

}

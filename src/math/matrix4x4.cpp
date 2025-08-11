// Copyright (c) 2025 Inan Evin
#include "matrix4x4.hpp"
#include "math.hpp"
#include "quat.hpp"

namespace SFG
{
	matrix4x4::matrix4x4(float m00,
						 float m10,
						 float m20,
						 float m30, // Col 0
						 float m01,
						 float m11,
						 float m21,
						 float m31, // Col 1
						 float m02,
						 float m12,
						 float m22,
						 float m32, // Col 2
						 float m03,
						 float m13,
						 float m23,
						 float m33) // Col 3
	{
		m[0]  = m00;
		m[1]  = m10;
		m[2]  = m20;
		m[3]  = m30;
		m[4]  = m01;
		m[5]  = m11;
		m[6]  = m21;
		m[7]  = m31;
		m[8]  = m02;
		m[9]  = m12;
		m[10] = m22;
		m[11] = m32;
		m[12] = m03;
		m[13] = m13;
		m[14] = m23;
		m[15] = m33;
	}

	const matrix4x4 matrix4x4::identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	matrix4x4 matrix4x4::get_normal_matrix() const
	{
		matrix4x4 inv_mat = inverse();
		if (inv_mat == matrix4x4::identity)
			return identity;
		return inv_mat.transpose();
	}

	matrix4x4 matrix4x4::transpose() const
	{
		matrix4x4 result;
		for (int i = 0; i < 4; ++i) // Col of result
		{
			for (int j = 0; j < 4; ++j) // Row of result
			{
				result.m[i * 4 + j] = m[j * 4 + i]; // Swap col and row indices
			}
		}
		return result;
	}

	float matrix4x4::determinant() const
	{
		float det;
		float a = m[0], b = m[4], c = m[8], d = m[12];
		float e = m[1], f = m[5], g = m[9], h = m[13];
		float i = m[2], j = m[6], k = m[10], l = m[14];
		float m_ = m[3], n = m[7], o = m[11], p = m[15];

		float kp_minus_lo = k * p - l * o;
		float jp_minus_ln = j * p - l * n;
		float jo_minus_kn = j * o - k * n;
		float ip_minus_lm = i * p - l * m_;
		float io_minus_km = i * o - k * m_;
		float in_minus_jm = i * n - j * m_;

		det = a * (f * kp_minus_lo - g * jp_minus_ln + h * jo_minus_kn) - b * (e * kp_minus_lo - g * ip_minus_lm + h * io_minus_km) + c * (e * jp_minus_ln - f * ip_minus_lm + h * in_minus_jm) - d * (e * jo_minus_kn - f * io_minus_km + g * in_minus_jm);

		return det;
	}

	matrix4x4 matrix4x4::inverse() const
	{
		float det = determinant();
		if (math::abs(det) < MATH_EPS)
			return identity;

		float	  inv_det = 1.0f / det;
		matrix4x4 inv;

		float a = m[0], b = m[4], c = m[8], d = m[12];
		float e = m[1], f = m[5], g = m[9], h = m[13];
		float i = m[2], j = m[6], k = m[10], l = m[14];
		float m_ = m[3], n = m[7], o = m[11], p = m[15];

		inv.m[0]  = (f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n)) * inv_det;
		inv.m[4]  = (-b * (k * p - l * o) + c * (j * p - l * n) - d * (j * o - k * n)) * inv_det;
		inv.m[8]  = (b * (g * p - h * o) - c * (f * p - h * n) + d * (f * o - g * n)) * inv_det;
		inv.m[12] = (-b * (g * l - h * k) + c * (f * l - h * j) - d * (f * k - g * j)) * inv_det;
		inv.m[1]  = (-e * (k * p - l * o) + g * (i * p - l * m_) - h * (i * o - k * m_)) * inv_det;
		inv.m[5]  = (a * (k * p - l * o) - c * (i * p - l * m_) + d * (i * o - k * m_)) * inv_det;
		inv.m[9]  = (-a * (g * p - h * o) + c * (e * p - h * m_) - d * (e * o - g * m_)) * inv_det;
		inv.m[13] = (a * (g * l - h * k) - c * (e * l - h * i) + d * (e * k - g * i)) * inv_det;
		inv.m[2]  = (e * (j * p - l * n) - f * (i * p - l * m_) + h * (i * n - j * m_)) * inv_det;
		inv.m[6]  = (-a * (j * p - l * n) + b * (i * p - l * m_) - d * (i * n - j * m_)) * inv_det;
		inv.m[10] = (a * (f * p - h * n) - b * (e * p - h * m_) + d * (e * n - f * m_)) * inv_det;
		inv.m[14] = (-a * (f * l - h * j) + b * (e * l - h * i) - d * (e * j - f * i)) * inv_det;
		inv.m[3]  = (-e * (j * o - k * n) + f * (i * o - k * m_) - g * (i * n - j * m_)) * inv_det;
		inv.m[7]  = (a * (j * o - k * n) - b * (i * o - k * m_) + c * (i * n - j * m_)) * inv_det;
		inv.m[11] = (-a * (f * o - g * n) + b * (e * o - g * m_) - c * (e * n - f * m_)) * inv_det;
		inv.m[15] = (a * (f * k - g * j) - b * (e * k - g * i) + c * (e * j - f * i)) * inv_det;

		return inv;
	}

	vector3 matrix4x4::get_scale() const
	{
		vector3 x_axis(m[0], m[1], m[2]);
		vector3 y_axis(m[4], m[5], m[6]);
		vector3 z_axis(m[8], m[9], m[10]);
		return vector3(x_axis.magnitude(), y_axis.magnitude(), z_axis.magnitude());
	}

	vector3 matrix4x4::get_translation() const
	{
		return vector3(m[12], m[13], m[14]);
	}

	matrix4x4 matrix4x4::translation(const vector3& t)
	{
		return matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, t.x, t.y, t.z, 1.0f);
	}

	matrix4x4 matrix4x4::scale(const vector3& s)
	{
		return matrix4x4(s.x, 0.0f, 0.0f, 0.0f, 0.0f, s.y, 0.0f, 0.0f, 0.0f, 0.0f, s.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	matrix4x4 matrix4x4::rotation(const quaternion& q)
	{
		float x2 = q.x * q.x;
		float y2 = q.y * q.y;
		float z2 = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;

		return matrix4x4(1.0f - 2.0f * (y2 + z2), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f, 2.0f * (xy - wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz + wx), 0.0f, 2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (x2 + y2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	matrix4x4 matrix4x4::ortho(float left, float right, float top, float bottom, float near_plane, float far_plane)
	{
		float inv_width	 = 1.0f / (right - left);
		float inv_height = 1.0f / (top - bottom);
		float inv_depth	 = -1.0f / (far_plane - near_plane);

		return matrix4x4(2.0f * inv_width, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f * inv_height, 0.0f, 0.0f, 0.0f, 0.0f, inv_depth, 0.0f, -(right + left) * inv_width, -(top + bottom) * inv_height, -near_plane * inv_depth, 1.0f);
	}

	matrix4x4 matrix4x4::perspective(float fov_y_degrees, float aspect_ratio, float near_plane, float far_plane)
	{
		float fov_rad	   = math::degrees_to_radians(fov_y_degrees);
		float tan_half_fov = math::tan(fov_rad * 0.5f);

		float inv_depth = 1.0f / (far_plane - near_plane);

		return matrix4x4(1.0f / (aspect_ratio * tan_half_fov), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f, 0.0f, 0.0f, far_plane * inv_depth, 1.0f, 0.0f, 0.0f, -near_plane * far_plane * inv_depth, 0.0f);
	}

	matrix4x4 matrix4x4::transform(const vector3& position, const quaternion& rotation, const vector3& scale_vec)
	{
		matrix4x4 mat_s = matrix4x4::scale(scale_vec);
		matrix4x4 mat_r = matrix4x4::rotation(rotation);
		matrix4x4 mat_t = matrix4x4::translation(position);

		return mat_t * mat_r * mat_s;
	}

	matrix4x4 matrix4x4::look_at(const vector3& eye, const vector3& target, const vector3& up_vec)
	{
		vector3 z_axis = (target - eye).normalized();
		vector3 x_axis = vector3::cross(up_vec, z_axis).normalized();
		vector3 y_axis = vector3::cross(z_axis, x_axis);

		matrix4x4 result = matrix4x4(x_axis.x, y_axis.x, z_axis.x, 0.0f, x_axis.y, y_axis.y, z_axis.y, 0.0f, x_axis.z, y_axis.z, z_axis.z, 0.0f, -vector3::dot(x_axis, eye), -vector3::dot(y_axis, eye), -vector3::dot(z_axis, eye), 1.0f);
		return result;
	}

	vector3 matrix4x4::operator*(const vector3& v) const
	{
		vector4 temp_v4(v.x, v.y, v.z, 1.0f);
		vector4 transformed_v4 = (*this) * temp_v4;
		if (math::abs(transformed_v4.w) < MATH_EPS)
			return vector3(transformed_v4.x, transformed_v4.y, transformed_v4.z);
		return vector3(transformed_v4.x / transformed_v4.w, transformed_v4.y / transformed_v4.w, transformed_v4.z / transformed_v4.w);
	}

	matrix4x4 matrix4x4::operator/(float scalar) const
	{
		matrix4x4 result;
		if (math::abs(scalar) < MATH_EPS)
			return identity;
		float inv_scalar = 1.0f / scalar;
		for (int i = 0; i < 16; ++i)
		{
			result.m[i] = m[i] * inv_scalar;
		}
		return result;
	}

	matrix4x4& matrix4x4::operator/=(float scalar)
	{
		if (math::abs(scalar) < MATH_EPS)
		{
			for (int i = 0; i < 16; ++i)
				m[i] = std::numeric_limits<float>::infinity();
		}
		else
		{
			float inv_scalar = 1.0f / scalar;
			for (int i = 0; i < 16; ++i)
			{
				m[i] *= inv_scalar;
			}
		}
		return *this;
	}

	bool matrix4x4::equals(const matrix4x4& other, float epsilon) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (!math::almost_equal(m[i], other.m[i], epsilon))
			{
				return false;
			}
		}
		return true;
	}

}
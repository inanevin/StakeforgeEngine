// Copyright (c) 2025 Inan Evin
#pragma once

#include "vector3.hpp"
#include "vector4.hpp"

namespace SFG
{
	class quat;

	class matrix4x4
	{
	public:
		float m[16]; // Column-major storage: m[col * 4 + row]

		matrix4x4() = default;
		matrix4x4(float m00,
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
				  float m33); // Col 3

		static const matrix4x4 identity;

		matrix4x4 get_normal_matrix() const;
		matrix4x4 transpose() const;
		float	  determinant() const;
		matrix4x4 inverse() const;
		vector3	  get_scale() const;
		vector3	  get_translation() const;
		bool	  equals(const matrix4x4& other, float epsilon = MATH_EPS) const;

		static matrix4x4 translation(const vector3& t);
		static matrix4x4 scale(const vector3& s);
		static matrix4x4 rotation(const quat& q);
		static matrix4x4 ortho(float left, float right, float top, float bottom, float near_plane, float far_plane);
		static matrix4x4 perspective(float fov_y_degrees, float aspect_ratio, float near_plane, float far_plane);
		static matrix4x4 transform(const vector3& position, const quat& rotation, const vector3& scale);
		static matrix4x4 look_at(const vector3& eye, const vector3& target, const vector3& up);

		inline float operator[](int index) const
		{
			return m[index];
		}
		inline float& operator[](int index)
		{
			return m[index];
		}

		inline bool operator==(const matrix4x4& other) const
		{
			return equals(other);
		}
		inline bool operator!=(const matrix4x4& other) const
		{
			return !equals(other);
		}

		inline matrix4x4 operator*(const matrix4x4& other) const
		{
			matrix4x4 result;
			for (int i = 0; i < 4; ++i) // Result columns
			{
				for (int j = 0; j < 4; ++j) // Result rows
				{
					result.m[i * 4 + j] = m[0 * 4 + j] * other.m[i * 4 + 0] + m[1 * 4 + j] * other.m[i * 4 + 1] + m[2 * 4 + j] * other.m[i * 4 + 2] + m[3 * 4 + j] * other.m[i * 4 + 3];
				}
			}
			return result;
		}

		inline vector4 operator*(const vector4& v) const
		{
			return vector4(m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w, m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w, m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w, m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w);
		}

		vector3 operator*(const vector3& v) const;

		inline matrix4x4 operator*(float scalar) const
		{
			matrix4x4 result;
			for (int i = 0; i < 16; ++i)
			{
				result.m[i] = m[i] * scalar;
			}
			return result;
		}

		matrix4x4 operator/(float scalar) const;

		inline matrix4x4& operator*=(const matrix4x4& other)
		{
			*this = (*this) * other;
			return *this;
		}

		inline matrix4x4& operator*=(float scalar)
		{
			for (int i = 0; i < 16; ++i)
			{
				m[i] *= scalar;
			}
			return *this;
		}

		matrix4x4& operator/=(float scalar);
	};

	inline matrix4x4 operator*(float scalar, const matrix4x4& mat)
	{
		return mat * scalar;
	}
}
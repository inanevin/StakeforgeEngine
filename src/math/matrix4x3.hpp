// Copyright (c) 2025 Inan Evin
#pragma once

#include "vector3.hpp"
#include "vector4.hpp"

namespace SFG
{
	class quat;
	class matrix4x4;

	class matrix4x3
	{
	public:
		float m[12]; // Column-major: m[col * 3 + row], 4 cols × 3 rows

		matrix4x3() = default;
		matrix4x3(float m00,
				  float m10,
				  float m20, // Col 0
				  float m01,
				  float m11,
				  float m21, // Col 1
				  float m02,
				  float m12,
				  float m22, // Col 2
				  float m03,
				  float m13,
				  float m23); // Col 3 (translation)

		static const matrix4x3 identity;

		vector3 get_translation() const;
		vector3 get_scale() const;

		static matrix4x3 translation(const vector3& t);
		static matrix4x3 scale(const vector3& s);
		static matrix4x3 rotation(const quat& q);
		static matrix4x3 transform(const vector3& position, const quat& rotation, const vector3& scale);

		matrix4x4		 to_matrix4x4() const;
		static matrix4x3 from_matrix4x4(const matrix4x4& mat);

		inline float operator[](int index) const
		{
			return m[index];
		}
		inline float& operator[](int index)
		{
			return m[index];
		}

		// Matrix × Matrix (composition)
		inline matrix4x3 operator*(const matrix4x3& other) const
		{
			matrix4x3 result;
			// 3x3 linear part
			for (int i = 0; i < 3; ++i) // row
			{
				for (int j = 0; j < 3; ++j) // col
				{
					result.m[j * 3 + i] = m[0 * 3 + i] * other.m[j * 3 + 0] + m[1 * 3 + i] * other.m[j * 3 + 1] + m[2 * 3 + i] * other.m[j * 3 + 2];
				}
			}
			// Translation
			for (int i = 0; i < 3; ++i)
			{
				result.m[3 * 3 + i] = m[0 * 3 + i] * other.m[9 + 0] + m[1 * 3 + i] * other.m[9 + 1] + m[2 * 3 + i] * other.m[9 + 2] + m[9 + i];
			}
			return result;
		}

		// Transform a point (applies rotation, scale, translation)
		inline vector3 operator*(const vector3& v) const
		{
			return vector3(m[0] * v.x + m[3] * v.y + m[6] * v.z + m[9], m[1] * v.x + m[4] * v.y + m[7] * v.z + m[10], m[2] * v.x + m[5] * v.y + m[8] * v.z + m[11]);
		}

		// Scale whole matrix
		inline matrix4x3 operator*(float scalar) const
		{
			matrix4x3 result;
			for (int i = 0; i < 12; ++i)
				result.m[i] = m[i] * scalar;
			return result;
		}

		inline matrix4x3& operator*=(float scalar)
		{
			for (int i = 0; i < 12; ++i)
				m[i] *= scalar;
			return *this;
		}
	};
}
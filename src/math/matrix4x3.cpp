// Copyright (c) 2025 Inan Evin
#include "matrix4x3.hpp"
#include "matrix4x4.hpp"
#include "math.hpp"
#include "quat.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{

	matrix4x3::matrix4x3(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22, float m03, float m13, float m23)
	{
		m[0]  = m00;
		m[1]  = m10;
		m[2]  = m20;
		m[3]  = m01;
		m[4]  = m11;
		m[5]  = m21;
		m[6]  = m02;
		m[7]  = m12;
		m[8]  = m22;
		m[9]  = m03;
		m[10] = m13;
		m[11] = m23;
	}

	const matrix4x3 matrix4x3::identity(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

	vector3 matrix4x3::get_translation() const
	{
		return vector3(m[9], m[10], m[11]);
	}

	vector3 matrix4x3::get_scale() const
	{
		vector3 x_axis(m[0], m[1], m[2]);
		vector3 y_axis(m[3], m[4], m[5]);
		vector3 z_axis(m[6], m[7], m[8]);
		return vector3(x_axis.magnitude(), y_axis.magnitude(), z_axis.magnitude());
	}

	matrix4x3 matrix4x3::translation(const vector3& t)
	{
		return matrix4x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, t.x, t.y, t.z);
	}

	matrix4x3 matrix4x3::scale(const vector3& s)
	{
		return matrix4x3(s.x, 0.0f, 0.0f, 0.0f, s.y, 0.0f, 0.0f, 0.0f, s.z, 0.0f, 0.0f, 0.0f);
	}

	matrix4x3 matrix4x3::rotation(const quat& q)
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

		return matrix4x3(1.0f - 2.0f * (y2 + z2), 2.0f * (xy + wz), 2.0f * (xz - wy), 2.0f * (xy - wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz + wx), 2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (x2 + y2), 0.0f, 0.0f, 0.0f);
	}

	matrix4x3 matrix4x3::transform(const vector3& position, const quat& rotation, const vector3& scale_vec)
	{
		matrix4x3 mat_s = matrix4x3::scale(scale_vec);
		matrix4x3 mat_r = matrix4x3::rotation(rotation);
		matrix4x3 mat_t = matrix4x3::translation(position);
		return mat_t * mat_r * mat_s;
	}

	matrix4x3 matrix4x3::inverse() const
	{
		matrix4x3 result;

		// 1. Get the squared inverse scale for the inversion of the 3x3 block
		// We assume get_scale() is already correct and non-zero
		vector3 s = get_scale();
		vector3 inv_s_sq(1.0f / (s.x * s.x), 1.0f / (s.y * s.y), 1.0f / (s.z * s.z));

		// For simplicity and robustness against extreme scale values, we'll decompose and invert explicitly:
		// M_inv_3x3 = (R*S)^-1 = S^-1 * R^-1 = S^-1 * R_T

		// The transpose of the Rotation part is needed (R_T)
		// R_T is stored in result.m[i*3 + j] where the original R was at m[j*3 + i]

		// 2. Compute the Inverse 3x3 Matrix (Rotation * Scale)^-1
		// M_inv_3x3 = M_3x3_T * S_inv_sq (THIS IS THE FAST WAY IF NO SHEAR IS PRESENT)
		// The columns of the original matrix are the scaled-rotated basis vectors.
		// The inverse is found by transposing and scaling by 1/(scale^2)

		// For a non-sheared matrix M = R * S, M_inv = S_inv * R_T.
		// However, since we don't have R and S separately, we use this:

		// Transpose the original 3x3 block (R*S)
		result.m[0] = m[0];
		result.m[1] = m[3];
		result.m[2] = m[6]; // Row 0 -> Col 0
		result.m[3] = m[1];
		result.m[4] = m[4];
		result.m[5] = m[7]; // Row 1 -> Col 1
		result.m[6] = m[2];
		result.m[7] = m[5];
		result.m[8] = m[8]; // Row 2 -> Col 2

		// Apply Inverse Scale Squared (to perform the M_inv_3x3 operation)
		// This is the fastest way to correctly invert the R*S block for a non-sheared matrix.
		result.m[0] *= inv_s_sq.x;
		result.m[1] *= inv_s_sq.x;
		result.m[2] *= inv_s_sq.x;
		result.m[3] *= inv_s_sq.y;
		result.m[4] *= inv_s_sq.y;
		result.m[5] *= inv_s_sq.y;
		result.m[6] *= inv_s_sq.z;
		result.m[7] *= inv_s_sq.z;
		result.m[8] *= inv_s_sq.z;

		// 3. Compute the Inverse Translation
		// t_inv = -M_inv_3x3 * t
		vector3 t(m[9], m[10], m[11]);

		// Use the resulting result.m[0..8] (the M_inv_3x3) to transform the original translation t
		vector3 inv_t;
		inv_t.x = result.m[0] * t.x + result.m[3] * t.y + result.m[6] * t.z;
		inv_t.y = result.m[1] * t.x + result.m[4] * t.y + result.m[7] * t.z;
		inv_t.z = result.m[2] * t.x + result.m[5] * t.y + result.m[8] * t.z;

		// Store the negative of the transformed translation
		result.m[9]	 = -inv_t.x;
		result.m[10] = -inv_t.y;
		result.m[11] = -inv_t.z;

		return result;
	}

	void matrix4x3::decompose(vector3& out_translation, quat& out_rotation, vector3& out_scale) const
	{
		// --- 1. Extract Translation ---
		out_translation = get_translation();

		// --- 2. Extract Scale ---
		vector3 x_axis(m[0], m[1], m[2]);
		vector3 y_axis(m[3], m[4], m[5]);
		vector3 z_axis(m[6], m[7], m[8]);

		out_scale.x = x_axis.magnitude();
		out_scale.y = y_axis.magnitude();
		out_scale.z = z_axis.magnitude();

		// Prevent divide by zero
		vector3 inv_scale(out_scale.x != 0.0f ? 1.0f / out_scale.x : 0.0f, out_scale.y != 0.0f ? 1.0f / out_scale.y : 0.0f, out_scale.z != 0.0f ? 1.0f / out_scale.z : 0.0f);

		// --- 3. Extract Rotation ---
		// Normalize the axes (remove scaling)
		vector3 nx = x_axis * inv_scale.x;
		vector3 ny = y_axis * inv_scale.y;
		vector3 nz = z_axis * inv_scale.z;

		float trace = nx.x + ny.y + nz.z;

		if (trace > 0.0f)
		{
			float s		   = sqrtf(trace + 1.0f) * 2.0f;
			out_rotation.w = 0.25f * s;
			out_rotation.x = (ny.z - nz.y) / s;
			out_rotation.y = (nz.x - nx.z) / s;
			out_rotation.z = (nx.y - ny.x) / s;
		}
		else if (nx.x > ny.y && nx.x > nz.z)
		{
			float s		   = sqrtf(1.0f + nx.x - ny.y - nz.z) * 2.0f;
			out_rotation.w = (ny.z - nz.y) / s;
			out_rotation.x = 0.25f * s;
			out_rotation.y = (ny.x + nx.y) / s;
			out_rotation.z = (nz.x + nx.z) / s;
		}
		else if (ny.y > nz.z)
		{
			float s		   = sqrtf(1.0f + ny.y - nx.x - nz.z) * 2.0f;
			out_rotation.w = (nz.x - nx.z) / s;
			out_rotation.x = (ny.x + nx.y) / s;
			out_rotation.y = 0.25f * s;
			out_rotation.z = (nz.y + ny.z) / s;
		}
		else
		{
			float s		   = sqrtf(1.0f + nz.z - nx.x - ny.y) * 2.0f;
			out_rotation.w = (nx.y - ny.x) / s;
			out_rotation.x = (nz.x + nx.z) / s;
			out_rotation.y = (nz.y + ny.z) / s;
			out_rotation.z = 0.25f * s;
		}

		out_rotation.normalize();
	}

	matrix4x4 matrix4x3::to_matrix4x4() const
	{
		// Fill 4x4 with affine data
		return matrix4x4(m[0],
						 m[1],
						 m[2],
						 0.0f, // Col 0
						 m[3],
						 m[4],
						 m[5],
						 0.0f, // Col 1
						 m[6],
						 m[7],
						 m[8],
						 0.0f, // Col 2
						 m[9],
						 m[10],
						 m[11],
						 1.0f // Col 3 (translation)
		);
	}

	matrix4x3 matrix4x3::from_matrix4x4(const matrix4x4& mat)
	{
		// Just copy the top-left 3x3 + translation column
		return matrix4x3(mat.m[0],
						 mat.m[1],
						 mat.m[2], // Col 0
						 mat.m[4],
						 mat.m[5],
						 mat.m[6], // Col 1
						 mat.m[8],
						 mat.m[9],
						 mat.m[10], // Col 2
						 mat.m[12],
						 mat.m[13],
						 mat.m[14] // Col 3
		);
	}

	void matrix4x3::serialize(ostream& stream) const
	{
		for (int i = 0; i < 12; ++i)
			stream << m[i];
	}
	void matrix4x3::deserialize(istream& stream)
	{
		for (int i = 0; i < 12; ++i)
			stream >> m[i];
	}
}
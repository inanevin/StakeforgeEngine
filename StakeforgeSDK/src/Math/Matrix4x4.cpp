/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Math/Matrix4x4.hpp"
#include "SFG/Math/Matrix3x3.hpp"
#include "SFG/Math/Quaternion.hpp"
#include "SFG/Math/Vector3.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Math/Transformation.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{
	Matrix4x4::Matrix4x4(const Quaternion& q)
	{
		float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
		float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
		float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

		m[0] = 1.0f - 2.0f * (yy + zz);
		m[1] = 2.0f * (xy - wz);
		m[2] = 2.0f * (xz + wy);
		m[3] = 0.0f;

		m[4] = 2.0f * (xy + wz);
		m[5] = 1.0f - 2.0f * (xx + zz);
		m[6] = 2.0f * (yz - wx);
		m[7] = 0.0f;

		m[8]  = 2.0f * (xz - wy);
		m[9]  = 2.0f * (yz + wx);
		m[10] = 1.0f - 2.0f * (xx + yy);
		m[11] = 0.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;
	}

	Matrix4x4::Matrix4x4(const float* data)
	{
		memcpy(m, data, 16 * sizeof(float));
	}

	Matrix4x4 Matrix4x4::Identity()
	{
		Matrix4x4 mat;
		memset(mat.m, 0, 16 * sizeof(float));
		mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
		return mat;
	}

	Matrix4x4 Matrix4x4::Translate(const Vector3& amt)
	{
		Matrix4x4 mat = Identity();
		mat.m[12]	  = amt.x;
		mat.m[13]	  = amt.y;
		mat.m[14]	  = amt.z;
		return mat;
	}

	Matrix4x4 Matrix4x4::Scale(const Vector3& amt)
	{
		Matrix4x4 mat = Identity();
		mat.m[0]	  = amt.x;
		mat.m[5]	  = amt.y;
		mat.m[10]	  = amt.z;
		return mat;
	}

	Matrix4x4 Matrix4x4::Scale(float amt)
	{
		return Scale(Vector3(amt, amt, amt));
	}

	Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float n, float f)
	{
		Matrix4x4 mat;
		memset(mat.m, 0, 16 * sizeof(float));

		mat.m[0]  = 2.0f / (right - left);
		mat.m[5]  = 2.0f / (top - bottom);
		mat.m[10] = 1.0f / (f - n); // Standard for Vulkan, but mapped to depth range 0 to 1
		mat.m[12] = -(right + left) / (right - left);
		mat.m[13] = -(top + bottom) / (top - bottom);
		mat.m[14] = -(f + n) / (f - n); // Changed to map to 0-1 depth range
		mat.m[15] = 1.0f;

		return mat;
	}

	Matrix4x4 Matrix4x4::Perspective(float halfFovDegrees, float aspect, float nearZ, float farZ)
	{
		float tanHalfFov = tanf(halfFovDegrees * (MATH_PI / 180.0f));

		Matrix4x4 mat;
		memset(mat.m, 0, 16 * sizeof(float));

		mat.m[0]  = 1.0f / (aspect * tanHalfFov);
		mat.m[5]  = 1.0f / tanHalfFov;
		mat.m[10] = farZ / (farZ - nearZ); // Standard for Vulkan, but maps to depth range 0 to 1
		mat.m[11] = 1.0f;
		mat.m[14] = -(nearZ * farZ) / (farZ - nearZ); // Adjusted to match 0-1 depth range
		mat.m[15] = 0.0f;

		return mat;
	}

	Matrix4x4 Matrix4x4::TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		Matrix4x4 trans = Matrix4x4::Translate(translation);
		Matrix4x4 rot	= Matrix4x4(rotation); // Assuming Quaternion-to-Matrix4 constructor exists
		Matrix4x4 sc	= Matrix4x4::Scale(scale);
		return trans * rot * sc;
	}

	Matrix4x4 Matrix4x4::Transposed() const
	{
		Matrix4x4 mat;
		mat.m[0]  = m[0];
		mat.m[1]  = m[4];
		mat.m[2]  = m[8];
		mat.m[3]  = m[12];
		mat.m[4]  = m[1];
		mat.m[5]  = m[5];
		mat.m[6]  = m[9];
		mat.m[7]  = m[13];
		mat.m[8]  = m[2];
		mat.m[9]  = m[6];
		mat.m[10] = m[10];
		mat.m[11] = m[14];
		mat.m[12] = m[3];
		mat.m[13] = m[7];
		mat.m[14] = m[11];
		mat.m[15] = m[15];
		return mat;
	}

	float Matrix4x4::Determinant4x4() const
	{
		float a = m[0], b = m[4], c = m[8], d = m[12];
		float e = m[1], f = m[5], g = m[9], h = m[13];
		float i = m[2], j = m[6], k = m[10], l = m[14];
		float m0 = m[3], n = m[7], o = m[11], p = m[15];

		return a * (f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n)) - b * (e * (k * p - l * o) - g * (i * p - l * m0) + h * (i * o - k * m0)) + c * (e * (j * p - l * n) - f * (i * p - l * m0) + h * (i * n - j * m0)) -
			   d * (e * (j * o - k * n) - f * (i * o - k * m0) + g * (i * n - j * m0));
	}

	float Matrix4x4::Determinant3x3() const
	{
		Matrix3x3 m3x3(m[0], m[4], m[8], m[1], m[5], m[9], m[2], m[6], m[10]);
		return m3x3.Determinant();
	}

	namespace
	{
		float determinant3x3(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
		{
			return m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31);
		}
	} // namespace
	Matrix4x4 Matrix4x4::Inverted() const
	{
		// Compute the determinant
		float det = Determinant4x4();
		if (fabs(det) < 1e-6f)	// Avoid division by zero
			return Matrix4x4(); // Return identity or some error indicator

		float	  invDet = 1.0f / det;
		Matrix4x4 inv;

		// Compute cofactors for each element
		inv.m[0] = determinant3x3(m[5], m[9], m[13], m[6], m[10], m[14], m[7], m[11], m[15]) * invDet;
		inv.m[1] = -determinant3x3(m[1], m[9], m[13], m[2], m[10], m[14], m[3], m[11], m[15]) * invDet;
		inv.m[2] = determinant3x3(m[1], m[5], m[13], m[2], m[6], m[14], m[3], m[7], m[15]) * invDet;
		inv.m[3] = -determinant3x3(m[1], m[5], m[9], m[2], m[6], m[10], m[3], m[7], m[11]) * invDet;

		inv.m[4] = -determinant3x3(m[4], m[8], m[12], m[6], m[10], m[14], m[7], m[11], m[15]) * invDet;
		inv.m[5] = determinant3x3(m[0], m[8], m[12], m[2], m[10], m[14], m[3], m[11], m[15]) * invDet;
		inv.m[6] = -determinant3x3(m[0], m[4], m[12], m[2], m[6], m[14], m[3], m[7], m[15]) * invDet;
		inv.m[7] = determinant3x3(m[0], m[4], m[8], m[2], m[6], m[10], m[3], m[7], m[11]) * invDet;

		inv.m[8]  = determinant3x3(m[4], m[8], m[12], m[5], m[9], m[13], m[7], m[11], m[15]) * invDet;
		inv.m[9]  = -determinant3x3(m[0], m[8], m[12], m[1], m[9], m[13], m[3], m[11], m[15]) * invDet;
		inv.m[10] = determinant3x3(m[0], m[4], m[12], m[1], m[5], m[13], m[3], m[7], m[15]) * invDet;
		inv.m[11] = -determinant3x3(m[0], m[4], m[8], m[1], m[5], m[9], m[3], m[7], m[11]) * invDet;

		inv.m[12] = -determinant3x3(m[4], m[8], m[12], m[5], m[9], m[13], m[6], m[10], m[14]) * invDet;
		inv.m[13] = determinant3x3(m[0], m[8], m[12], m[1], m[9], m[13], m[2], m[10], m[14]) * invDet;
		inv.m[14] = -determinant3x3(m[0], m[4], m[12], m[1], m[5], m[13], m[2], m[6], m[14]) * invDet;
		inv.m[15] = determinant3x3(m[0], m[4], m[8], m[1], m[5], m[9], m[2], m[6], m[10]) * invDet;

		return inv;
	}

	Matrix4x4 Matrix4x4::InitRotation(const Quaternion& quat)
	{
		float xx2 = 2.0f * quat.x * quat.x;
		float yy2 = 2.0f * quat.y * quat.y;
		float zz2 = 2.0f * quat.z * quat.z;
		float xy2 = 2.0f * quat.x * quat.y;
		float xz2 = 2.0f * quat.x * quat.z;
		float yz2 = 2.0f * quat.y * quat.z;
		float wx2 = 2.0f * quat.w * quat.x;
		float wy2 = 2.0f * quat.w * quat.y;
		float wz2 = 2.0f * quat.w * quat.z;

		Matrix4x4 result;
		result.m[0] = -yy2 - zz2 + 1.0f;
		result.m[1] = xy2 + wz2;
		result.m[2] = xz2 - wy2;
		result.m[3] = 0.0f;

		result.m[4] = xy2 - wz2;
		result.m[5] = -xx2 - zz2 + 1.0f;
		result.m[6] = yz2 + wx2;
		result.m[7] = 0.0f;

		result.m[8]	 = xz2 + wy2;
		result.m[9]	 = yz2 - wx2;
		result.m[10] = -xx2 - yy2 + 1.0f;
		result.m[11] = 0.0f;

		result.m[12] = 0.0f;
		result.m[13] = 0.0f;
		result.m[14] = 0.0f;
		result.m[15] = 1.0f;

		return result;
	}

	Matrix4x4 Matrix4x4::InitLookAt(const Vector3& location, const Vector3& center, const Vector3& up)
	{
		Vector3 forward = (center - location).Normalized(); // Left-handed: positive Z points forward
		Vector3 right	= up.Cross(forward).Normalized();
		Vector3 newUp	= forward.Cross(right); // Ensures newUp is perpendicular

		Matrix4x4 result;
		result.m[0] = right.x;
		result.m[1] = newUp.x;
		result.m[2] = forward.x;
		result.m[3] = 0.0f;

		result.m[4] = right.y;
		result.m[5] = newUp.y;
		result.m[6] = forward.y;
		result.m[7] = 0.0f;

		result.m[8]	 = right.z;
		result.m[9]	 = newUp.z;
		result.m[10] = forward.z;
		result.m[11] = 0.0f;

		result.m[12] = -right.Dot(location);
		result.m[13] = -newUp.Dot(location);
		result.m[14] = -forward.Dot(location);
		result.m[15] = 1.0f;

		return result;
	}

	Matrix4x4 Matrix4x4::Scaled(const Vector3& scale)
	{
		Matrix4x4 result;
		result.m[0]	 = scale.x;
		result.m[5]	 = scale.y;
		result.m[10] = scale.z;
		result.m[15] = 1.0f;
		return (*this) * result; // Column-major: multiplication applies transformation
	}

	Vector3 Matrix4x4::GetScale() const
	{
		return Vector3(sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]), sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]), sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]));
	}

	Vector3 Matrix4x4::GetTranslation() const
	{
		return Vector3(m[12], m[13], m[14]);
	}

	void Matrix4x4::Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const
	{
		position = GetTranslation();
		scale	 = GetScale();

		// Normalize the rotation matrix by removing the scale
		Matrix4x4 rotationMatrix = *this;
		rotationMatrix.m[0] /= scale.x;
		rotationMatrix.m[1] /= scale.x;
		rotationMatrix.m[2] /= scale.x;

		rotationMatrix.m[4] /= scale.y;
		rotationMatrix.m[5] /= scale.y;
		rotationMatrix.m[6] /= scale.y;

		rotationMatrix.m[8] /= scale.z;
		rotationMatrix.m[9] /= scale.z;
		rotationMatrix.m[10] /= scale.z;

		// Convert rotation matrix to quaternion
		float trace = rotationMatrix.m[0] + rotationMatrix.m[5] + rotationMatrix.m[10];
		if (trace > 0.0f)
		{
			float s	   = 0.5f / sqrt(trace + 1.0f);
			rotation.w = 0.25f / s;
			rotation.x = (rotationMatrix.m[9] - rotationMatrix.m[6]) * s;
			rotation.y = (rotationMatrix.m[2] - rotationMatrix.m[8]) * s;
			rotation.z = (rotationMatrix.m[4] - rotationMatrix.m[1]) * s;
		}
		else
		{
			if (rotationMatrix.m[0] > rotationMatrix.m[5] && rotationMatrix.m[0] > rotationMatrix.m[10])
			{
				float s	   = 2.0f * sqrt(1.0f + rotationMatrix.m[0] - rotationMatrix.m[5] - rotationMatrix.m[10]);
				rotation.w = (rotationMatrix.m[9] - rotationMatrix.m[6]) / s;
				rotation.x = 0.25f * s;
				rotation.y = (rotationMatrix.m[1] + rotationMatrix.m[4]) / s;
				rotation.z = (rotationMatrix.m[2] + rotationMatrix.m[8]) / s;
			}
			else if (rotationMatrix.m[5] > rotationMatrix.m[10])
			{
				float s	   = 2.0f * sqrt(1.0f + rotationMatrix.m[5] - rotationMatrix.m[0] - rotationMatrix.m[10]);
				rotation.w = (rotationMatrix.m[2] - rotationMatrix.m[8]) / s;
				rotation.x = (rotationMatrix.m[1] + rotationMatrix.m[4]) / s;
				rotation.y = 0.25f * s;
				rotation.z = (rotationMatrix.m[6] + rotationMatrix.m[9]) / s;
			}
			else
			{
				float s	   = 2.0f * sqrt(1.0f + rotationMatrix.m[10] - rotationMatrix.m[0] - rotationMatrix.m[5]);
				rotation.w = (rotationMatrix.m[4] - rotationMatrix.m[1]) / s;
				rotation.x = (rotationMatrix.m[2] + rotationMatrix.m[8]) / s;
				rotation.y = (rotationMatrix.m[6] + rotationMatrix.m[9]) / s;
				rotation.z = 0.25f * s;
			}
		}
	}

	Transformation Matrix4x4::ToTransform()
	{
		Vector3	   position, scale;
		Quaternion rotation = Quaternion::Identity();
		Decompose(position, rotation, scale);
		return Transformation(position, rotation, scale);
	}

	void Matrix4x4::SaveToStream(OStream& stream) const
	{
		for (int i = 0; i < 16; i++)
		{
			stream << m[i];
		}
	}

	void Matrix4x4::LoadFromStream(IStream& stream)
	{
		for (int i = 0; i < 16; i++)
		{
			stream >> m[i];
		}
	}

} // namespace SFG

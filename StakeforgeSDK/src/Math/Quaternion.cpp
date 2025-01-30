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

#include "SFG/Math/Quaternion.hpp"
#include "SFG/Math/Vector3.hpp"
#include "SFG/Math/Vector4.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Math/Matrix3x3.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{
	Quaternion Quaternion::Identity()
	{
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Quaternion Quaternion::Rotation(const Vector3& forward, const Vector3& axis)
	{
		Vector3 normalizedForward = forward.Normalized();
		Vector3 normalizedAxis	  = axis.Normalized();

		float dot	= normalizedForward.Dot(normalizedAxis);
		float angle = acos(dot); // Angle between the two vectors

		Vector3 rotationAxis = normalizedForward.Cross(normalizedAxis).Normalized();

		// Quaternion is based on axis-angle rotation
		float halfAngle = angle * 0.5f;
		float s			= sin(halfAngle);
		return Quaternion(cos(halfAngle), rotationAxis.x * s, rotationAxis.y * s, rotationAxis.z * s);
	}

	Quaternion Quaternion::FromVector(const Vector3& rot)
	{
		float halfRoll	= rot.x * 0.5f;
		float halfPitch = rot.y * 0.5f;
		float halfYaw	= rot.z * 0.5f;

		float cosRoll  = cos(halfRoll);
		float sinRoll  = sin(halfRoll);
		float cosPitch = cos(halfPitch);
		float sinPitch = sin(halfPitch);
		float cosYaw   = cos(halfYaw);
		float sinYaw   = sin(halfYaw);

		return Quaternion(
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw, sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw, cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw, cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw);
	}

	Quaternion Quaternion::ShortMix(const Quaternion& q1, const Quaternion& q2, float alpha)
	{
		float dot = q1.Dot(q2);

		if (dot < 0.0f)
		{
			return Lerp(q1, -q2, alpha);
		}
		return Lerp(q1, q2, alpha);
	}

	Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float alpha)
	{
		Quaternion result = q1 + (q2 - q1) * alpha;
		return result.Normalized();
	}

	Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& dest, float t)
	{
		float		dot		  = from.Dot(dest);
		const float THRESHOLD = 0.9995f;

		if (dot > THRESHOLD)
		{
			return Lerp(from, dest, t);
		}

		dot			  = Math::Clamp(dot, -1.0f, 1.0f);
		float theta_0 = Math::Acos(dot);
		float theta	  = theta_0 * t;

		Quaternion destTemp = (dest - from * dot).Normalized();
		return from * Math::Cos(theta) + destTemp * Math::Sin(theta);
	}

	Quaternion Quaternion::PitchYawRoll(const Vector3& v)
	{
		float pitch = v.x * 0.5f;
		float yaw	= v.y * 0.5f;
		float roll	= v.z * 0.5f;

		float cosPitch = Math::Cos(pitch);
		float sinPitch = Math::Sin(pitch);
		float cosYaw   = Math::Cos(yaw);
		float sinYaw   = Math::Sin(yaw);
		float cosRoll  = Math::Cos(roll);
		float sinRoll  = Math::Sin(roll);

		return Quaternion(
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw, sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw, cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw, cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw);
	}

	Quaternion Quaternion::PitchYawRoll(float x, float y, float z)
	{
		return Quaternion::PitchYawRoll(Vector3(x, y, z));
	}

	Quaternion Quaternion::AngleAxis(float angleDegrees, const Vector3& axis)
	{
		float angle = RAD_2_DEG * angleDegrees * 0.5f;
		float s		= Math::Sin(angle);
		return Quaternion(Math::Cos(angle), axis.x * s, axis.y * s, axis.z * s);
	}

	Quaternion Quaternion::Normalized() const
	{
		const float len = Magnitude();
		return Quaternion(x / len, y / len, z / len, w / len);
	}

	bool Quaternion::IsNormalized() const
	{
		return Math::Abs(1.0f - MagnitudeSquared()) < 1.e-4f;
	}

	Quaternion Quaternion::FromMatrix(const Matrix3x3& mat)
	{
		float trace = mat.m[0] + mat.m[4] + mat.m[8]; // sum of the diagonal elements
		float w, x, y, z;

		if (trace > 0.0f)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			w		= 0.25f / s;
			x		= (mat.m[5] - mat.m[7]) * s;
			y		= (mat.m[6] - mat.m[2]) * s;
			z		= (mat.m[1] - mat.m[3]) * s;
		}
		else
		{
			if (mat.m[0] > mat.m[4] && mat.m[0] > mat.m[8])
			{
				float s = 2.0f * sqrtf(1.0f + mat.m[0] - mat.m[4] - mat.m[8]);
				w		= (mat.m[5] - mat.m[7]) / s;
				x		= 0.25f * s;
				y		= (mat.m[3] + mat.m[1]) / s;
				z		= (mat.m[6] + mat.m[2]) / s;
			}
			else if (mat.m[4] > mat.m[8])
			{
				float s = 2.0f * sqrtf(1.0f + mat.m[4] - mat.m[0] - mat.m[8]);
				w		= (mat.m[6] - mat.m[2]) / s;
				x		= (mat.m[3] + mat.m[1]) / s;
				y		= 0.25f * s;
				z		= (mat.m[7] + mat.m[5]) / s;
			}
			else
			{
				float s = 2.0f * sqrtf(1.0f + mat.m[8] - mat.m[0] - mat.m[4]);
				w		= (mat.m[1] - mat.m[3]) / s;
				x		= (mat.m[6] + mat.m[2]) / s;
				y		= (mat.m[7] + mat.m[5]) / s;
				z		= 0.25f * s;
			}
		}

		return Quaternion(w, x, y, z);
	}

	Vector3 Quaternion::GetRight() const
	{
		return GetRotated(Vector3::Right);
	}

	Vector3 Quaternion::GetUp() const
	{
		return GetRotated(Vector3::Up);
	}

	Vector3 Quaternion::GetForward() const
	{
		return GetRotated(Vector3::Forward);
	}

	Vector3 Quaternion::GetPitchYawRoll() const
	{
		float sinr_cosp = 2.0f * (w * x + y * z);
		float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
		float roll		= Math::Atan2(sinr_cosp, cosr_cosp); // Roll (x-axis rotation)

		float sinp	= 2.0f * (w * y - z * x);
		float pitch = Math::FAbs(sinp) >= 1.0f ? Math::CopySign(MATH_PI / 2.0f, sinp) : Math::Asin(sinp); // Pitch (y-axis rotation)

		float siny_cosp = 2.0f * (w * z + x * y);
		float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
		float yaw		= Math::Atan2(siny_cosp, cosy_cosp); // Yaw (z-axis rotation)

		return Vector3(roll, pitch, yaw); // Return as Vector3: pitch, yaw, roll
	}

	Vector3 Quaternion::GetAxis() const
	{
		float angle = 2.0f * acos(w);
		float s		= sqrt(1.0f - w * w); // Sin of the angle

		// Prevent division by zero
		if (s < 0.001f)
		{
			return Vector3(1.0f, 0.0f, 0.0f); // Default axis
		}

		return Vector3(x / s, y / s, z / s); // Axis of rotation
	}

	float Quaternion::GetAngle() const
	{
		return 2.0f * Math::Acos(w);
	}

	Vector3 Quaternion::GetRotated(const Vector3& other) const
	{
		Quaternion qConjugate = Conjugate();		   // Get conjugate of the quaternion
		Quaternion qVec(0, other.x, other.y, other.z); // Convert vector to quaternion form

		// Apply rotation: q * v * q^-1
		Quaternion qResult = *this * qVec * qConjugate;

		return Vector3(qResult.x, qResult.y, qResult.z); // Return the rotated vector
	}

	Quaternion Quaternion::LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
	{
		// Step 1: Calculate the forward vector (direction to look)
		Vector3 forward = (to - from).Normalized(); // Look towards the target

		// Step 2: Calculate the right vector (perpendicular to forward and up)
		Vector3 right = up.Cross(forward).Normalized();

		// Step 3: Calculate the up vector (perpendicular to right and forward)
		Vector3 correctedUp = forward.Cross(right).Normalized();

		// Step 4: Construct the rotation matrix (column-major)
		Matrix3x3 rotationMatrix;
		rotationMatrix.SetColumn(0, right);		  // Right
		rotationMatrix.SetColumn(1, correctedUp); // Up
		rotationMatrix.SetColumn(2, -forward);	  // Negative forward (because of left-handed system)

		// Step 5: Convert the rotation matrix to a quaternion
		return Quaternion::FromMatrix(rotationMatrix);
	}

	Quaternion Quaternion::Conjugate() const
	{
		// Conjugate of a quaternion (invert the axis, keep the angle)
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion Quaternion::Inverted() const
	{
		// Inverse of a quaternion (conjugate divided by length squared)
		float lenSq = MagnitudeSquared();
		if (lenSq > 0.0f)
		{
			float invLenSq = 1.0f / lenSq;
			return Quaternion(-x * invLenSq, -y * invLenSq, -z * invLenSq, w * invLenSq);
		}
		return Quaternion::Identity();
	}

	float Quaternion::Dot(const Quaternion& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float Quaternion::Magnitude() const
	{
		return Math::Sqrt(MagnitudeSquared());
	}

	float Quaternion::MagnitudeSquared() const
	{
		return w * w + x * x + y * y + z * z;
	}

	Vector4 Quaternion::ToVector() const
	{
		return Vector4(x, y, z, w);
	}

	void Quaternion::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}

	void Quaternion::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

} // namespace SFG

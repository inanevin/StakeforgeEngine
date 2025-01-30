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

#pragma once

namespace SFG
{
	class Vector3;
	class Vector4;
	class OStream;
	class IStream;
	class Matrix3x3;

	class Quaternion
	{

	public:
		Quaternion() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		Quaternion(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {};

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Quaternion Identity();

		/// <summary>
		///
		/// </summary>
		/// <param name="forward"></param>
		/// <param name="axis"></param>
		/// <returns></returns>
		static Quaternion Rotation(const Vector3& forward, const Vector3& axis);

		/// <summary>
		///
		/// </summary>
		/// <param name="rot"></param>
		/// <returns></returns>
		static Quaternion FromVector(const Vector3& rot);

		/// <summary>
		///
		/// </summary>
		/// <param name="q1"></param>
		/// <param name="q2"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static Quaternion ShortMix(const Quaternion& q1, const Quaternion& q2, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <param name="q1"></param>
		/// <param name="q2"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <param name="from"></param>
		/// <param name="dest"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		static Quaternion Slerp(const Quaternion& from, const Quaternion& dest, float t);

		/// <summary>
		///
		/// </summary>
		/// <param name="v"></param>
		/// <returns></returns>
		static Quaternion PitchYawRoll(const Vector3& v);

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <returns></returns>
		static Quaternion PitchYawRoll(float x, float y, float z);

		/// <summary>
		///
		/// </summary>
		/// <param name="angleDegrees"></param>
		/// <param name="axis"></param>
		/// <returns></returns>
		static Quaternion AngleAxis(float angleDegrees, const Vector3& axis);

		/// <summary>
		///
		/// </summary>
		/// <param name="from"></param>
		/// <param name="to"></param>
		/// <param name="up"></param>
		/// <returns></returns>
		static Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up);

		/// <summary>
		///
		/// </summary>
		/// <param name="mat"></param>
		/// <returns></returns>
		static Quaternion FromMatrix(const Matrix3x3& mat);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetRight() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetUp() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetForward() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetPitchYawRoll() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetAxis() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector3 GetRotated(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Quaternion Conjugate() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Quaternion Inverted() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Quaternion Normalized() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		bool IsNormalized() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float GetAngle() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Dot(const Quaternion& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Magnitude() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float MagnitudeSquared() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector4 ToVector() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void SaveToStream(OStream& stream) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void LoadFromStream(IStream& stream);

		Quaternion operator+(const Quaternion& other) const
		{
			return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
		}
		Quaternion operator-(const Quaternion& other) const
		{
			return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
		}
		Quaternion operator*(float amt) const
		{
			return Quaternion(x * amt, y * amt, z * amt, w * amt);
		}

		Quaternion operator+=(const Quaternion& other)
		{
			*this += other;
			return *this;
		}
		Quaternion operator-=(const Quaternion& other)
		{
			*this -= other;
			return *this;
		}
		Quaternion operator*=(const Quaternion& other)
		{
			*this *= other;
			return *this;
		}
		Quaternion operator*=(float amt)
		{
			*this *= amt;
			return *this;
		}
		Quaternion operator/=(float amt)
		{
			*this /= amt;
			return *this;
		}
		Quaternion operator=(const Quaternion& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
			return *this;
		}

		Quaternion operator*(const Quaternion& other) const
		{
			float w = this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z;
			float x = this->w * other.x + this->x * other.w + this->y * other.z - this->z * other.y;
			float y = this->w * other.y - this->x * other.z + this->y * other.w + this->z * other.x;
			float z = this->w * other.z + this->x * other.y - this->y * other.x + this->z * other.w;

			return Quaternion(w, x, y, z);
		}

		Quaternion operator-() const
		{
			return Quaternion(-x, -y, -z, -w);
		}
		bool operator==(const Quaternion& other) const
		{
			return *this == other;
		}
		bool operator!=(const Quaternion& other) const
		{
			return *this != other;
		}

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

} // namespace SFG

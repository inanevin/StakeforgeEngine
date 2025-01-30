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
	class IStream;
	class OStream;
	class Vector2;

	class Vector3
	{

	public:
		Vector3() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		Vector3 Clamp(const Vector3& min, const Vector3& max) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector3 Cross(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 Abs() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector3 Min(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector3 Max(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 Normalized() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <returns></returns>
		Vector3 Project(const Vector3& normal) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		/// <returns></returns>
		Vector3 Rotate(const Vector3& axis, float angle) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="rotation"></param>
		/// <returns></returns>
		Vector3 Rotate(const class Quaternion& rotation) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <returns></returns>
		Vector3 Reflect(const Vector3& normal) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <param name="indexOfRefraction"></param>
		/// <returns></returns>
		Vector3 Refract(const Vector3& normal, float indexOfRefraction) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Dot(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Distance(const Vector3& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float DistanceSqrt(const Vector3& other) const;

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
		float Max() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Min() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Avg() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="epsilon"></param>
		/// <returns></returns>
		bool Equals(const Vector3& other, float epsilon) const;

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

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 XY() const;

		Vector3& operator*=(Vector3 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			return *this;
		}
		Vector3& operator/=(Vector3 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			return *this;
		}
		Vector3& operator+=(Vector3 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}
		Vector3& operator-=(Vector3 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}
		Vector3& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			return *this;
		}
		Vector3& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			this->z /= f;
			return *this;
		}
		Vector3& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			this->z += f;
			return *this;
		}
		Vector3& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			this->z -= f;
			return *this;
		}
		bool operator==(const Vector3& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
		bool operator!=(const Vector3& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z);
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}

		static Vector3 Zero;
		static Vector3 Up;
		static Vector3 Down;
		static Vector3 Right;
		static Vector3 Forward;
		static Vector3 One;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	inline Vector3 operator*(float scalar, Vector3 const& v)
	{
		return Vector3(scalar * v.x, scalar * v.y, scalar * v.z);
	}
	inline Vector3 operator/(float scalar, Vector3 const& v)
	{
		return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
	}
	inline Vector3 operator+(float scalar, Vector3 const& v)
	{
		return Vector3(scalar + v.x, scalar + v.y, scalar + v.z);
	}
	inline Vector3 operator-(float scalar, Vector3 const& v)
	{
		return Vector3(scalar - v.x, scalar - v.y, scalar - v.z);
	}
	inline Vector3 operator*(Vector3 const& v, float scalar)
	{
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}
	inline Vector3 operator/(Vector3 const& v, float scalar)
	{
		return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
	}
	inline Vector3 operator+(Vector3 const& v, float scalar)
	{
		return Vector3(v.x + scalar, v.y + scalar, v.z + scalar);
	}
	inline Vector3 operator-(Vector3 const& v, float scalar)
	{
		return Vector3(v.x - scalar, v.y - scalar, v.z - scalar);
	}
	inline Vector3 operator*(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x * v2.x, v.y * v2.y, v.z * v2.z);
	}
	inline Vector3 operator/(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x / v2.x, v.y / v2.y, v.z / v2.z);
	}
	inline Vector3 operator+(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x + v2.x, v.y + v2.y, v.z + v2.z);
	}
	inline Vector3 operator-(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x - v2.x, v.y - v2.y, v.z - v2.z);
	}

} // namespace SFG

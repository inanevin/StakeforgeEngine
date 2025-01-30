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
	class Color;
	class Vector2;
	class Vector3;

	class Vector4
	{

	public:
		Vector4() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="c"></param>
		Vector4(const Color& c);

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		Vector4 Clamp(const Vector4& min, const Vector4& max) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector4 Abs() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector4 Min(const Vector4& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector4 Max(const Vector4& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector4 Normalized() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Dot(const Vector4& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Distance(const Vector4& other) const;

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
		bool Equals(const Vector4& other, float epsilon) const;

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
		Vector3 XYZ() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 XY() const;

		Vector4& operator*=(Vector4 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			this->w *= v.w;
			return *this;
		}
		Vector4& operator/=(Vector4 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			this->w /= v.w;
			return *this;
		}
		Vector4& operator+=(Vector4 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			this->w += v.w;
			return *this;
		}
		Vector4& operator-=(Vector4 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			this->w -= v.w;
			return *this;
		}
		Vector4& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			this->w *= f;
			return *this;
		}
		Vector4& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			this->z /= f;
			this->w /= f;
			return *this;
		}
		Vector4& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			this->z += f;
			this->w += f;
			return *this;
		}
		Vector4& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			this->z -= f;
			this->w -= f;
			return *this;
		}
		bool operator==(const Vector4& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
		bool operator!=(const Vector4& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector4 operator-() const
		{
			return Vector4(-*this);
		}

		static Vector4 Zero;
		static Vector4 One;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

	inline Vector4 operator*(float scalar, Vector4 const& v)
	{
		return Vector4(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
	}
	inline Vector4 operator/(float scalar, Vector4 const& v)
	{
		return Vector4(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
	}
	inline Vector4 operator+(float scalar, Vector4 const& v)
	{
		return Vector4(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w);
	}
	inline Vector4 operator-(float scalar, Vector4 const& v)
	{
		return Vector4(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w);
	}
	inline Vector4 operator*(Vector4 const& v, float scalar)
	{
		return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
	}
	inline Vector4 operator/(Vector4 const& v, float scalar)
	{
		return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
	}
	inline Vector4 operator+(Vector4 const& v, float scalar)
	{
		return Vector4(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar);
	}
	inline Vector4 operator-(Vector4 const& v, float scalar)
	{
		return Vector4(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar);
	}
	inline Vector4 operator*(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x * v2.x, v.y * v2.y, v.z * v2.z, v.w * v2.w);
	}
	inline Vector4 operator/(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x / v2.x, v.y / v2.y, v.z / v2.z, v.w * v2.w);
	}
	inline Vector4 operator+(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x + v2.x, v.y + v2.y, v.z + v2.z, v.w * v2.w);
	}
	inline Vector4 operator-(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w * v2.w);
	}
} // namespace SFG

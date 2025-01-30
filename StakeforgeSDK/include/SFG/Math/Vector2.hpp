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
	class Vector2i;

	class Vector2
	{
	public:
		/// <summary>
		///
		/// </summary>
		Vector2() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		Vector2(float x_, float y_) : x(x_), y(y_) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="rhs"></param>
		Vector2(const Vector2i& rhs);

		/// <summary>
		///
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		Vector2 Clamp(const Vector2& min, const Vector2& max) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="mag"></param>
		/// <returns></returns>
		Vector2 ClampMagnitude(float mag) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 Abs() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector2 Min(const Vector2& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector2 Max(const Vector2& other) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 Normalized() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <returns></returns>
		Vector2 Project(const Vector2& normal) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="degrees"></param>
		/// <returns></returns>
		Vector2 Rotate(float degrees) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <returns></returns>
		Vector2 Reflect(const Vector2& normal) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="normal"></param>
		/// <param name="indexOfRefraction"></param>
		/// <returns></returns>
		Vector2 Refract(const Vector2& normal, float indexOfRefraction) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="epsilon"></param>
		/// <returns></returns>
		bool Equals(const Vector2& other, float epsilon) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="marg"></param>
		/// <returns></returns>
		bool IsZero(float marg = 0.0001f) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Dot(const Vector2& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Distance(const Vector2& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float DistanceSqrt(const Vector2& other) const;

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
		/// <returns></returns>
		float Angle(const Vector2& other) const;

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

		Vector2& operator*=(Vector2 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			return *this;
		}
		Vector2& operator/=(Vector2 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			return *this;
		}
		Vector2& operator+=(Vector2 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		Vector2& operator-=(Vector2 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}
		Vector2& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			return *this;
		}
		Vector2& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			return *this;
		}
		Vector2& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			return *this;
		}
		Vector2& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			return *this;
		}
		bool operator==(const Vector2& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}
		bool operator!=(const Vector2& rhs) const
		{
			return !(x == rhs.x && y == rhs.y);
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector2 operator-() const
		{
			return Vector2(-*this);
		}
		float* Get()
		{
			return &x;
		}

		static Vector2 Zero;
		static Vector2 One;

		float x = 0.0f;
		float y = 0.0f;
	};

	inline Vector2 operator*(float scalar, Vector2 const& v)
	{
		return Vector2(scalar * v.x, scalar * v.y);
	}
	inline Vector2 operator/(float scalar, Vector2 const& v)
	{
		return Vector2(scalar / v.x, scalar / v.y);
	}
	inline Vector2 operator+(float scalar, Vector2 const& v)
	{
		return Vector2(scalar + v.x, scalar + v.y);
	}
	inline Vector2 operator-(float scalar, Vector2 const& v)
	{
		return Vector2(scalar - v.x, scalar - v.y);
	}
	inline Vector2 operator*(Vector2 const& v, float scalar)
	{
		return Vector2(v.x * scalar, v.y * scalar);
	}
	inline Vector2 operator/(Vector2 const& v, float scalar)
	{
		return Vector2(v.x / scalar, v.y / scalar);
	}
	inline Vector2 operator+(Vector2 const& v, float scalar)
	{
		return Vector2(v.x + scalar, v.y + scalar);
	}
	inline Vector2 operator-(Vector2 const& v, float scalar)
	{
		return Vector2(v.x - scalar, v.y - scalar);
	}
	inline Vector2 operator*(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x * v2.x, v.y * v2.y);
	}
	inline Vector2 operator/(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x / v2.x, v.y / v2.y);
	}
	inline Vector2 operator+(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x + v2.x, v.y + v2.y);
	}
	inline Vector2 operator-(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x - v2.x, v.y - v2.y);
	}
} // namespace SFG

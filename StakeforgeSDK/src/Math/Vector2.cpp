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

#include "SFG/Math/Vector2.hpp"
#include "SFG/Math/Vector2i.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Data/OStream.hpp"

namespace SFG
{

	Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
	Vector2 Vector2::One  = Vector2(1.0f, 0.0f);

	//////////////////////////////////////////////

	Vector2::Vector2(const Vector2i& rhs) : x(static_cast<float>(rhs.x)), y(static_cast<float>(rhs.y))
	{
	}

	Vector2 Vector2::Clamp(const Vector2& min, const Vector2& max) const
	{
		return Vector2(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y));
	}

	Vector2 Vector2::ClampMagnitude(float mag) const
	{
		if (Magnitude() > mag)
			return Normalized() * mag;
		return *this;
	}

	Vector2 Vector2::Abs() const
	{
		return Vector2(Math::Abs(x), Math::Abs(y));
	}

	Vector2 Vector2::Min(const Vector2& other) const
	{
		return Vector2(Math::Min(x, other.x), Math::Min(y, other.y));
	}

	Vector2 Vector2::Max(const Vector2& other) const
	{
		return Vector2(Math::Max(x, other.x), Math::Max(y, other.y));
	}

	Vector2 Vector2::Normalized() const
	{
		if (IsZero())
			return Vector2::Zero;

		float len = Magnitude();
		return Vector2(x / len, y / len);
	}

	Vector2 Vector2::Project(const Vector2& normal) const
	{
		float dotProduct	= Dot(normal);
		float normalSquared = normal.MagnitudeSquared();
		return normal * (dotProduct / normalSquared);
	}

	Vector2 Vector2::Rotate(float degrees) const
	{
		float rad	   = degrees * DEG_2_RAD;
		float cosAngle = Math::Cos(rad);
		float sinAngle = Math::Sin(rad);
		return Vector2(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle);
	}

	Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		float dotProduct = Dot(normal);
		return *this - normal * (2.0f * dotProduct);
	}

	Vector2 Vector2::Refract(const Vector2& normal, float indexOfRefraction) const
	{
		float dotProduct = Dot(normal);
		float k			 = 1.0f - indexOfRefraction * indexOfRefraction * (1.0f - dotProduct * dotProduct);
		if (k < 0.0f)
		{
			return Zero; // Total internal reflection
		}
		return *this * indexOfRefraction - normal * (indexOfRefraction * dotProduct + std::sqrt(k));
	}

	bool Vector2::Equals(const Vector2& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon);
	}

	bool Vector2::IsZero(float marg) const
	{
		return Math::Equals(x, 0.0f, marg) && Math::Equals(x, 0.0f, marg);
	}

	float Vector2::Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	float Vector2::Distance(const Vector2& other) const
	{
		return std::sqrt(DistanceSqrt(other));
	}

	float Vector2::DistanceSqrt(const Vector2& other) const
	{
		return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
	}

	float Vector2::Magnitude() const
	{
		return Math::Sqrt(MagnitudeSquared());
	}

	float Vector2::MagnitudeSquared() const
	{
		return x * x + y * y;
	}

	float Vector2::Max() const
	{
		return Math::Max(x, y);
	}

	float Vector2::Min() const
	{
		return Math::Min(x, y);
	}

	float Vector2::Avg() const
	{
		return (x + y) / 2.0f;
	}

	float Vector2::Angle(const Vector2& other) const
	{
		float angle = RAD_2_DEG * (atan2(other.y, other.x) - atan2(y, x));

		return angle;
	}

	void Vector2::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}
} // namespace SFG

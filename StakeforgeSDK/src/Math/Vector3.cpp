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

#include "SFG/Math/Vector3.hpp"
#include "SFG/Math/Vector2.hpp"
#include "SFG/Math/Quaternion.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Data/OStream.hpp"

namespace SFG
{

	Vector3 Vector3::Zero	 = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 Vector3::One	 = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 Vector3::Forward = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 Vector3::Up		 = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 Vector3::Right	 = Vector3(1.0f, 0.0f, 0.0f);

	Vector3 Vector3::Clamp(const Vector3& min, const Vector3& max) const
	{
		return Vector3(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y), Math::Clamp(this->z, min.z, max.z));
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	Vector3 Vector3::Abs() const
	{
		return Vector3(Math::Abs(x), Math::Abs(y), Math::Abs(z));
	}

	Vector3 Vector3::Min(const Vector3& other) const
	{
		return Vector3(Math::Min(x, other.x), Math::Min(y, other.y), Math::Min(z, other.z));
	}

	Vector3 Vector3::Max(const Vector3& other) const
	{
		return Vector3(Math::Max(x, other.x), Math::Max(y, other.y), Math::Max(z, other.z));
	}

	Vector3 Vector3::Normalized() const
	{
		float len = Magnitude();
		if (len == 0.0f)
			return *this; // Prevent division by zero
		return Vector3(x / len, y / len, z / len);
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		float dotProduct	= Dot(normal);
		float normalSquared = normal.MagnitudeSquared();
		return normal * (dotProduct / normalSquared);
	}

	Vector3 Vector3::Rotate(const Vector3& axis, float angle) const
	{
		float	cosAngle	 = std::cos(angle);
		float	sinAngle	 = std::sin(angle);
		float	dotProduct	 = Dot(axis);
		Vector3 crossProduct = Cross(axis);

		// Rodrigues' rotation formula
		return *this * cosAngle + crossProduct * sinAngle + axis * (dotProduct * (1 - cosAngle));
	}

	Vector3 Vector3::Rotate(const Quaternion& rotation) const
	{
		Quaternion qv(0, x, y, z); // Represent vector as a quaternion (w = 0)
		Quaternion qConjugate = rotation.Conjugate();
		Quaternion result	  = rotation * qv * qConjugate; // Apply quaternion rotation
		return Vector3(result.x, result.y, result.z);
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		float dotProduct = Dot(normal);
		return *this - normal * (2.0f * dotProduct);
	}

	Vector3 Vector3::Refract(const Vector3& normal, float indexOfRefraction) const
	{
		float dotProduct = Dot(normal);
		float k			 = 1.0f - indexOfRefraction * indexOfRefraction * (1.0f - dotProduct * dotProduct);
		if (k < 0.0f)
		{
			return Vector3(0.0f, 0.0f, 0.0f); // Total internal reflection
		}
		return *this * indexOfRefraction - normal * (indexOfRefraction * dotProduct + std::sqrt(k));
	}

	bool Vector3::Equals(const Vector3& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon);
	}

	void Vector3::SaveToStream(OStream& stream) const
	{
		stream << x << y << z;
	}

	void Vector3::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z;
	}

	Vector2 Vector3::XY() const
	{
		return Vector2(x, y);
	}

	float Vector3::Dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float Vector3::Distance(const Vector3& other) const
	{
		return Math::Sqrt(DistanceSqrt(other));
	}

	float Vector3::DistanceSqrt(const Vector3& other) const
	{
		const Vector3 v = *this - other;
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	float Vector3::Magnitude() const
	{
		return Math::Sqrt(MagnitudeSquared());
	}

	float Vector3::MagnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

	float Vector3::Max() const
	{
		return Math::Max(x, Math::Max(y, z));
	}

	float Vector3::Min() const
	{
		return Math::Min(x, Math::Min(y, z));
	}

	float Vector3::Avg() const
	{
		return (x + y + z) / 3.0f;
	}

} // namespace SFG

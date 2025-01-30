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

#include "SFG/Math/Vector4.hpp"
#include "SFG/Math/Vector2.hpp"
#include "SFG/Math/Vector3.hpp"
#include "SFG/Math/Quaternion.hpp"
#include "SFG/Math/Color.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Data/OStream.hpp"

namespace SFG
{
	Vector4 Vector4::Zero = Vector4(0.0f);
	Vector4 Vector4::One  = Vector4(1.0f);

	Vector4::Vector4(const Color& c)
	{
		x = c.x;
		y = c.y;
		z = c.z;
		w = c.w;
	}

	Vector4 Vector4::Clamp(const Vector4& min, const Vector4& max) const
	{
		return Vector4(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y), Math::Clamp(this->z, min.z, max.z), Math::Clamp(this->w, min.w, max.w));
	}

	Vector4 Vector4::Abs() const
	{
		return Vector4(Math::Abs(x), Math::Abs(y), Math::Abs(z), Math::Abs(w));
	}

	Vector4 Vector4::Min(const Vector4& other) const
	{
		return Vector4(Math::Min(x, other.x), Math::Min(y, other.y), Math::Min(z, other.z), Math::Min(w, other.w));
	}

	Vector4 Vector4::Max(const Vector4& other) const
	{
		return Vector4(Math::Max(x, other.x), Math::Max(y, other.y), Math::Max(z, other.z), Math::Max(w, other.w));
	}

	Vector4 Vector4::Normalized() const
	{
		float len = Magnitude();
		if (len == 0.0f)
			return *this; // Prevent division by zero
		return Vector4(x / len, y / len, z / len, w / len);
	}

	float Vector4::Dot(const Vector4& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float Vector4::Distance(const Vector4& other) const
	{
		float dx = x - other.x;
		float dy = y - other.y;
		float dz = z - other.z;
		float dw = w - other.w;
		return Math::Sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
	}

	float Vector4::Magnitude() const
	{
		return Math::Sqrt(MagnitudeSquared());
	}

	float Vector4::MagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Vector4::Max() const
	{
		return Math::Max(x, Math::Max(y, Math::Max(z, w)));
	}

	float Vector4::Min() const
	{
		return Math::Min(x, Math::Min(y, Math::Min(z, w)));
	}

	float Vector4::Avg() const
	{
		return (x + y + z + w) / 4.0f;
	}

	bool Vector4::Equals(const Vector4& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon) && Math::Equals(w, other.w, epsilon);
	}

	void Vector4::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}

	void Vector4::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}
	Vector3 Vector4::XYZ() const
	{
		return Vector3(x, y, z);
	}
	Vector2 Vector4::XY() const
	{
		return Vector2(x, y);
	}
} // namespace SFG

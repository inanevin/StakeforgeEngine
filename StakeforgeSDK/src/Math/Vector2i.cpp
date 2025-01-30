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

#include "SFG/Math/Vector2i.hpp"
#include "SFG/Math/Vector2.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Data/OStream.hpp"

namespace SFG
{
	Vector2i Vector2i::Zero = Vector2i(0, 0);
	Vector2i Vector2i::One	= Vector2i(1, 1);

	Vector2i::Vector2i(const Vector2& v2)
	{
		x = static_cast<int32>(v2.x);
		y = static_cast<int32>(v2.y);
	}

	bool Vector2i::Equals(const Vector2i& other, int epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon);
	}

	void Vector2i::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2i::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}
} // namespace SFG

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

#include "SFG/Math/Vector2ui.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{
	Vector2ui Vector2ui::Zero = Vector2ui(0, 0);
	Vector2ui Vector2ui::One  = Vector2ui(1, 1);

	Vector2ui Vector2ui::Min(const Vector2ui& other) const
	{
		return Vector2ui(Math::Min(x, other.x), Math::Min(y, other.y));
	}

	Vector2ui Vector2ui::Max(const Vector2ui& other) const
	{
		return Vector2ui(Math::Max(x, other.x), Math::Max(y, other.y));
	}
	bool Vector2ui::Equals(const Vector2ui& other) const
	{
		return x == other.x && y == other.y;
	}

	void Vector2ui::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2ui::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}

} // namespace SFG

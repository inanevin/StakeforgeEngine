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

#include "SFG/Math/Vector3i.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"

namespace SFG
{
	Vector3i Vector3i::Zero = Vector3i(0, 0, 0);
	Vector3i Vector3i::One	= Vector3i(1, 1, 1);

	bool Vector3i::Equals(const Vector3i& other, int epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon);
	}

	void Vector3i::SaveToStream(OStream& stream) const
	{
		stream << x << y << z;
	}

	void Vector3i::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z;
	}
} // namespace SFG

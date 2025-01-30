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

#include "SFG/Math/Matrix3x3.hpp"
#include "SFG/Math/Vector3.hpp"

namespace SFG
{
	Matrix3x3 SFG::Matrix3x3::Rotation(float angleDegrees)
	{
		return Matrix3x3();
	}

	Matrix3x3 Matrix3x3::Identity()
	{
		return Matrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	void Matrix3x3::SetColumn(int col, const Vector3& vec)
	{
		m[col]	   = vec.x;
		m[col + 3] = vec.y;
		m[col + 6] = vec.z;
	}

	float Matrix3x3::Determinant() const
	{
		return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[3] * (m[1] * m[8] - m[2] * m[7]) + m[6] * (m[1] * m[5] - m[2] * m[4]);
	}

	Vector3 Matrix3x3::operator*(const Vector3& vec) const
	{
		Vector3 result;
		result.x = (*this)(0, 0) * vec.x + (*this)(1, 0) * vec.y + (*this)(2, 0) * vec.z;
		result.y = (*this)(0, 1) * vec.x + (*this)(1, 1) * vec.y + (*this)(2, 1) * vec.z;
		result.z = (*this)(0, 2) * vec.x + (*this)(1, 2) * vec.y + (*this)(2, 2) * vec.z;
		return result;
	}

} // namespace SFG

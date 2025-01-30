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
	class Vector3;

	class Matrix3x3
	{
	public:
		Matrix3x3()
		{
			for (int i = 0; i < 9; ++i)
				m[i] = 0.0f;
		}

		Matrix3x3(float val)
		{
			for (int i = 0; i < 9; ++i)
				m[i] = val;
		}

		Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
		{
			m[0] = m00;
			m[1] = m10;
			m[2] = m20;
			m[3] = m01;
			m[4] = m11;
			m[5] = m21;
			m[6] = m02;
			m[7] = m12;
			m[8] = m22;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="angleDegrees"></param>
		/// <returns></returns>
		static Matrix3x3 Rotation(float angleDegrees);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Matrix3x3 Identity();

		/// <summary>
		///
		/// </summary>
		/// <param name="col"></param>
		/// <param name="vec"></param>
		void SetColumn(int col, const Vector3& vec);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Determinant() const;

		Vector3 operator*(const Vector3& vec) const;

		float& operator()(int row, int col)
		{
			return m[row + col * 3];
		}

		const float& operator()(int row, int col) const
		{
			return m[row + col * 3];
		}

		Matrix3x3 operator*(const Matrix3x3& other) const
		{
			Matrix3x3 result;
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					result(i, j) = 0.0f;
					for (int k = 0; k < 3; ++k)
					{
						result(i, j) += (*this)(k, j) * other(i, k);
					}
				}
			}
			return result;
		}

		float m[9] = {0.0f};
	};

} // namespace SFG

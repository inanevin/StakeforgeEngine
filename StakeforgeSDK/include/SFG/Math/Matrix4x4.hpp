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
	class Transformation;
	class Quaternion;
	class Vector3;
	class Vector4;
	class IStream;
	class OStream;

	class Matrix4x4
	{
	public:
		/// <summary>
		///
		/// </summary>
		Matrix4x4() {};

		/// <summary>
		///
		/// </summary>
		/// <param name="q"></param>
		Matrix4x4(const Quaternion& q);

		/// <summary>
		///
		/// </summary>
		/// <param name="data"></param>
		Matrix4x4(const float* data);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Matrix4x4 Identity();

		/// <summary>
		///
		/// </summary>
		/// <param name="amt"></param>
		/// <returns></returns>
		static Matrix4x4 Translate(const Vector3& amt);

		/// <summary>
		///
		/// </summary>
		/// <param name="amt"></param>
		/// <returns></returns>
		static Matrix4x4 Scale(const Vector3& amt);

		/// <summary>
		///
		/// </summary>
		/// <param name="amt"></param>
		/// <returns></returns>
		static Matrix4x4 Scale(float amt);

		/// <summary>
		///
		/// </summary>
		/// <param name="left"></param>
		/// <param name="right"></param>
		/// <param name="bottom"></param>
		/// <param name="top"></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <returns></returns>
		static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		///
		/// </summary>
		/// <param name="halfFovDegrees"></param>
		/// <param name="aspect"></param>
		/// <param name="nearZ"></param>
		/// <param name="farZ"></param>
		/// <returns></returns>
		static Matrix4x4 Perspective(float halfFovDegrees, float aspect, float nearZ, float farZ);

		/// <summary>
		///
		/// </summary>
		/// <param name="translation"></param>
		/// <param name="rotation"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		static Matrix4x4 TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);

		/// <summary>
		///
		/// </summary>
		/// <param name="q"></param>
		/// <returns></returns>
		static Matrix4x4 InitRotation(const Quaternion& q);

		/// <summary>
		///
		/// </summary>
		/// <param name="location"></param>
		/// <param name="center"></param>
		/// <param name="up"></param>
		/// <returns></returns>
		static Matrix4x4 InitLookAt(const Vector3& location, const Vector3& center, const Vector3& up);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Determinant4x4() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		float Determinant3x3() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Matrix4x4 Transposed() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Matrix4x4 Inverted() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="scale"></param>
		/// <returns></returns>
		Matrix4x4 Scaled(const Vector3& scale);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetScale() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector3 GetTranslation() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="position"></param>
		/// <param name="rotation"></param>
		/// <param name="scale"></param>
		void Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Transformation ToTransform();

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

		Matrix4x4 operator*(const Matrix4x4& other) const
		{
			Matrix4x4 result;

			for (int col = 0; col < 4; col++)
			{
				for (int row = 0; row < 4; row++)
					result.m[col * 4 + row] = m[0 * 4 + row] * other.m[col * 4 + 0] + m[1 * 4 + row] * other.m[col * 4 + 1] + m[2 * 4 + row] * other.m[col * 4 + 2] + m[3 * 4 + row] * other.m[col * 4 + 3];
			}

			return result;
		}

		float m[16] = {0.0f};
	};

} // namespace SFG

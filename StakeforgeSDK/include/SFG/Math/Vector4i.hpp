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

#include "SFG/Type/SizeDefinitions.hpp"

namespace SFG
{
	class IStream;
	class OStream;

	class Vector4i
	{
	public:
		/// <summary>
		///
		/// </summary>
		Vector4i() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		Vector4i(int32 x_, int32 y_, int32 z_, int32 w_) : x(x_), y(y_), z(z_), w(w_) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="epsilon"></param>
		/// <returns></returns>
		bool Equals(const Vector4i& other, int epsilon = 0) const;

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

		static Vector4i Zero;
		static Vector4i One;

		int32 x = 0;
		int32 y = 0;
		int32 z = 0;
		int32 w = 0;
	};

	inline Vector4i operator-(Vector4i const& v, Vector4i const& v2)
	{
		return Vector4i(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w - v2.w);
	}
	inline Vector4i operator+(Vector4i const& v, float scalar)
	{
		return Vector4i((int)((float)v.x + scalar), (int)(((float)v.y + scalar)), (int)((float)v.z + scalar), (int)((float)v.w + scalar));
	}
	inline Vector4i operator-(Vector4i const& v, float scalar)
	{
		return Vector4i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)), (int)(((float)v.z - scalar)), (int)(((float)v.w - scalar)));
	}

} // namespace SFG

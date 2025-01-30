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

	class Vector4ui16
	{
	public:
		/// <summary>
		///
		/// </summary>
		Vector4ui16() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="w"></param>
		Vector4ui16(uint16 x_, uint16 y_, uint16 z_, uint16 w_) : x(x_), y(y_), z(z_), w(w_) {};

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

		uint16 x = 0;
		uint16 y = 0;
		uint16 z = 0;
		uint16 w = 0;
	};

	inline Vector4ui16 operator-(Vector4ui16 const& v, Vector4ui16 const& v2)
	{
		return Vector4ui16(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w - v2.w);
	}
	inline Vector4ui16 operator+(Vector4ui16 const& v, float scalar)
	{
		return Vector4ui16((int)((float)v.x + scalar), (int)(((float)v.y + scalar)), (int)((float)v.z + scalar), (int)((float)v.w + scalar));
	}
	inline Vector4ui16 operator-(Vector4ui16 const& v, float scalar)
	{
		return Vector4ui16((int)((float)v.x - scalar), (int)(((float)v.y - scalar)), (int)(((float)v.z - scalar)), (int)(((float)v.w - scalar)));
	}

} // namespace SFG

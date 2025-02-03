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
    class Vector2;
	class Vector2ui;

	class Vector2i
	{
	public:
		/// <summary>
		///
		/// </summary>
		Vector2i() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		Vector2i(int32 x_, int32 y_) : x(x_), y(y_){};

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		Vector2i(const Vector2& v2);
        
        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        Vector2i(const Vector2ui& v2);

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="epsilon"></param>
		/// <returns></returns>
		bool Equals(const Vector2i& other, int epsilon = 0) const;

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
        
        /// <summary>
        ///
        /// </summary>
        /// <param name="other"></param>
        /// <param name="v1"></param>
        /// <param name="v2"></param>
        /// <returns></returns>
        static Vector2i Clamp(const Vector2i& val, const Vector2i& v1, const Vector2i& v2);

		Vector2i& operator+=(Vector2i const& v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		Vector2i& operator-=(Vector2i const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}

		static Vector2i Zero;
		static Vector2i One;

		int32 x = 0;
		int32 y = 0;
	};

	inline Vector2i operator-(Vector2i const& v, Vector2i const& v2)
	{
		return Vector2i(v.x - v2.x, v.y - v2.y);
	}
	inline Vector2i operator+(Vector2i const& v, Vector2i const& v2)
	{
		return Vector2i(v.x + v2.x, v.y + v2.y);
	}
	inline Vector2i operator+(Vector2i const& v, float scalar)
	{
		return Vector2i((int)((float)v.x + scalar), (int)(((float)v.y + scalar)));
	}
	inline Vector2i operator-(Vector2i const& v, float scalar)
	{
		return Vector2i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)));
	}
	inline Vector2i operator*(Vector2i const& v, float scalar)
	{
		return Vector2i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)));
	}

} // namespace SFG

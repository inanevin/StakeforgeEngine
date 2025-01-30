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

#include "SFG/Math/Vector2.hpp"
#include "SFG/Math/Vector3.hpp"

namespace SFG
{
	class Rect
	{
	public:
		Rect() {};

		/// <summary>
		///
		/// </summary>
		/// <param name="p"></param>
		/// <param name="s"></param>
		Rect(const Vector2& p, const Vector2& s) : pos(p), size(s) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="w"></param>
		/// <param name="h"></param>
		Rect(float x, float y, float w, float h) : pos(x, y), size(w, h) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="percentage"></param>
		/// <returns></returns>
		Rect Shrinked(float percentage) const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 GetCenter() const;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		Vector2 GetEnd() const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		bool IsClippingHorizontal(const Rect& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		bool IsClippingVertical(const Rect& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		bool IsClipping(const Rect& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="p"></param>
		/// <param name="equalOK"></param>
		/// <returns></returns>
		bool IsPointInside(const Vector2& p, bool equalOK = false) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="p"></param>
		/// <param name="borderThickness"></param>
		/// <param name="border"></param>
		/// <returns></returns>
		bool IsInBorder(const Vector2& p, float borderThickness, int& border) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="margin"></param>
		/// <returns></returns>
		bool IsRectInside(const Rect& other, float margin = 0.0f) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <param name="margin"></param>
		/// <returns></returns>
		bool IsRectCompletelyOutside(const Rect& other, float margin = 0.0f) const;

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

		Vector2 pos	 = Vector2(0, 0);
		Vector2 size = Vector2(0, 0);
	};

	inline Rect operator+(Rect const& a, Rect const& b)
	{
		return Rect(a.pos.x + b.pos.x, a.pos.y + b.pos.y, a.size.x + b.size.x, a.size.y + b.size.y);
	}

} // namespace SFG

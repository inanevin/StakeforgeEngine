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

	class Vector2ui
	{
	public:
		/// <summary>
		///
		/// </summary>
		Vector2ui() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		Vector2ui(unsigned int x_, unsigned int y_) : x(x_), y(y_) {};

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector2ui Min(const Vector2ui& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vector2ui Max(const Vector2ui& other) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		bool Equals(const Vector2ui& other) const;

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

		static Vector2ui Zero;
		static Vector2ui One;

		uint32 x = 0;
		uint32 y = 0;
	};

} // namespace SFG

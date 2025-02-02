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

#include "SFG/Data/String.hpp"
#include "SFG/StakeforgeAPI.hpp"

namespace SFG
{
	class Color;

	class SFG_API ColorUtils
	{

	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="hex"></param>
		static Color FromHex(const String& hex);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static String ToHex(const Color& color);

		/// <summary>
		/// Calculate angle from hue-saturation and conver
		/// </summary>
		/// <returns></returns>
		static Color HS2SRGB(const Color& color);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Color SRGB2HSV(const Color& color);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Color HSV2SRGB(const Color& color);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Color SRGB2Linear(const Color& color);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Color Linear2SRGB(const Color& color);

		/// <summary>
		/// Lerps to white by amt.
		/// </summary>
		/// <param name="amt"></param>
		/// <returns></returns>
		static Color Brighten(const Color& color, float amt);

		/// <summary>
		/// Lerps to black by amt.
		/// </summary>
		/// <returns></returns>
		static Color Darken(const Color& color, float amt);
	};

} // namespace SFG

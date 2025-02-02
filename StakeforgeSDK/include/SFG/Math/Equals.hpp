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
	class Equals
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline bool NaN(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7FFFFFFF) > 0x7F800000;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline bool Finite(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7F800000) != 0x7F800000;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="errorMargin"></param>
		/// <returns></returns>
		template <typename T> static bool Value(const T& val1, const T& val2, const T& errorMargin)
		{
			const float t = val1 - val2;
			return (t < 0.0f ? -t : t) < errorMargin;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <returns></returns>
		template <typename T> static bool Zero(const T& val, float marg = 0.00001f)
		{
			const float t = val - (T)0;
			return (t < 0.0f ? -t : t) <= (T)marg;
		}
	};
} // namespace SFG

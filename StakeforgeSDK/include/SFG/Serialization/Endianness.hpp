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

#ifdef SFG_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 28251)
#pragma warning(disable : 6001)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <bit>
#include <algorithm>
#include <array>

namespace SFG
{
	class Endianness
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <param name=""></param>
		template <typename T> static inline void SwapEndian(T& val, typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type = nullptr)
		{
			union U {
				T									val;
				std::array<std::uint8_t, sizeof(T)> raw;
			} src, dst;

			src.val = val;
			std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
			val = dst.val;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static inline bool ShouldSwap()
		{
			return std::endian::native == std::endian::big;
		}
	};

} // namespace SFG

#ifdef SFG_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
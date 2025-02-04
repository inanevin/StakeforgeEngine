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
	class BumpAllocator
	{
	public:
		BumpAllocator() = delete;
		BumpAllocator(size_t sz);
		~BumpAllocator();

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		uint8* Allocate(size_t size);

		/// <summary>
		///
		/// </summary>
		inline void Reset()
		{
			m_head = 0;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template <typename T, typename... Args> T* Allocate(Args&&... args)
		{
			uint8* ptr = Allocate(sizeof(T));
			new ((void)ptr) T(std::forward<Args>(args...));
			return ptr;
		}

	private:
		size_t m_size = 0;
		uint32 m_head = 0;
		uint8* m_raw  = nullptr;
	};
} // namespace SFG

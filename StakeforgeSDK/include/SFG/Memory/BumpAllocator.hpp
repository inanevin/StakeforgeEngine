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
#include "SFG/IO/Assert.hpp"
#include "SFG/StakeforgeAPI.hpp"
#include <new>

namespace SFG
{
	class SFG_API BumpAllocator
	{
	public:
		BumpAllocator(size_t sz);
		~BumpAllocator();

		BumpAllocator()										 = delete;
		BumpAllocator& operator=(const BumpAllocator& other) = delete;
		BumpAllocator(const BumpAllocator& other)			 = delete;

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		void* Allocate(size_t size, size_t alignment = 1);

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
		template <typename T, typename... Args> T* Allocate(size_t count, Args&&... args)
		{
			void* ptr	   = Allocate(sizeof(T) * count, std::alignment_of<T>::value);
			T*	  arrayPtr = reinterpret_cast<T*>(ptr);
			for (size_t i = 0; i < count; ++i)
				new (&arrayPtr[i]) T(std::forward<Args>(args)...);
			return arrayPtr;
		}

		/// <summary>
		///
		/// </summary>
		template <typename T, typename... Args> T* EmplaceAux(T firstValue, Args&&... remainingValues)
		{
			uint8* initialHead = (uint8*)m_raw + m_head;

			// Place the first value in memory
			uint8* currentHead = initialHead;
			SFG_MEMCPY(currentHead, &firstValue, sizeof(T));
			m_head += sizeof(T);
			SFG_ASSERT(m_head < m_size);

			// Recursively place the remaining values in memory
			if constexpr (sizeof...(remainingValues) > 0)
			{
				EmplaceAux<T>(remainingValues...);
			}

			return reinterpret_cast<T*>(initialHead);
		}

	private:
		size_t m_size = 0;
		size_t m_head = 0;
		void*  m_raw  = nullptr;
	};
} // namespace SFG

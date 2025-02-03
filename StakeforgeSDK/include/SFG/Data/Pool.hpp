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
#include "SFG/Data/Handle.hpp"
#include "SFG/Memory/Memory.hpp"

namespace SFG
{
	template <typename T, int N, bool ALIGN_CACHE = false> class Pool
	{
	public:
		Pool()
		{
			static_assert(N != 0);

			m_size = N;
			Reserve();

			for (uint32 i = 0; i < N; i++)
			{
				m_gens[i]		  = 1;
				m_stackIndices[i] = i;
			}
		}

		~Pool()
		{
			if (ALIGN_CACHE)
				ALIGNED_FREE(m_raw);
			else
				FREE(m_raw);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template <typename... Args> Handle<T> Create(Args&&... args)
		{
			if (m_stackIndicesPos >= N)
			{
				Grow();
			}

			Handle<T> handle;
			handle.m_index		= m_stackIndices[m_stackIndicesPos];
			handle.m_generation = m_gens[handle.m_index];
			m_stackIndicesPos++;

			const void* ptr = (void*)(m_raw + handle.GetIndex());
			new (ptr) T(std::forward<Args>(args)...);

			return handle;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="handle"></param>
		void Free(const Handle<T>& handle)
		{
			SFG_ASSERT(handle.Alive(), "");
			const uint32 index = handle.GetIndex();
			m_raw[index].~T();
			m_gens[index]++;
			m_stackIndicesPos--;
			m_stackIndices[m_stackIndicesPos] = index;
		}

		/// <summary>
		///
		/// </summary>
		void IsEmpty() const
		{
			m_stackIndicesPos = 0;
		}

		/// <summary>
		///
		/// </summary>
		void GetSize() const
		{
			return m_stackIndicesPos;
		}

	private:
		void Reserve()
		{
			const size_t perElement = sizeof(T) + sizeof(uint16) + sizeof(uint32);
			const size_t cache		= 64;
			const size_t totalSize	= ALIGN_CACHE ? (((perElement * N + (cache - 1)) / cache) * cache) : (perElement * m_size);
			m_raw = reinterpret_cast<T*>(ALIGN_CACHE ? (ALIGNED_MALLOC(totalSize, cache) : MALLOC(totalSize)));
			m_gens					= reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(m_raw) + (m_size * sizeof(T)));
			m_stackIndices			= reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(m_raw) + m_size * (sizeof(T) + sizeof(uint16)));
		}

		void Grow()
		{
			T*		prev	  = m_raw;
			uint16* prevGens  = m_gens;
			uint32* prevStack = m_stackIndices;
			uint32	prevSize  = m_size;

			m_size *= 2;
			Reserve();

			MEMCPY(m_raw, prev, prevSize * sizeof(T));
			MEMCPY(m_gens, prevGens, prevSize * sizeof(uint16));

			for (uint32 i = prevSize; i < m_size; i++)
			{
				m_gens[i]		  = 1;
				m_stackIndices[i] = i;
			}
		}

	private:
		T*		m_raw			  = nullptr;
		uint16* m_gens			  = nullptr;
		uint32* m_stackIndices	  = nullptr;
		uint32	m_stackIndicesPos = 0;
		uint32	m_size			  = 0;
	};
} // namespace SFG

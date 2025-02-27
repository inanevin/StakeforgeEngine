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
	template <typename T, typename IndexType, int N, size_t ALIGN> class PoolAllocator
	{
	public:
		PoolAllocator()
		{
			static_assert((ALIGN & (ALIGN - 1)) == 0, "ALIGN must be a power of 2");
			static_assert(N != 0);

			m_size = N;
			Reserve();
		}

		~PoolAllocator()
		{
			SFG_ALIGNED_FREE(m_raw);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template <typename... Args> Handle<IndexType> Allocate(Args&&... args)
		{
			if (m_stackIndicesPos >= m_size)
			{
				Grow();
			}

			const uint32			index  = m_stackIndices[m_stackIndicesPos];
			const uint32			gen	   = m_gens[index];
			const Handle<IndexType> handle = Handle<IndexType>(index, gen);
			m_stackIndicesPos++;

			void* ptr = (void*)(m_raw + handle.GetIndex());
			new (ptr) T(std::forward<Args>(args)...);

			return handle;
		}

		/// <summary>
		///
		/// </summary>
		inline T& Get(Handle<IndexType> handle)
		{
			SFG_ASSERT(handle.Alive());
			SFG_ASSERT(m_gens[handle.GetIndex()] == handle.GetGeneration());
			return m_raw[handle.GetIndex()];
		}
		/// <summary>
		///
		/// </summary>
		/// <param name="handle"></param>
		inline void Free(Handle<IndexType> handle)
		{
			SFG_ASSERT(handle.Alive());
			const uint32 index = handle.GetIndex();

			m_raw[index].~T();
			m_gens[index]++;
			m_stackIndicesPos--;
			m_stackIndices[m_stackIndicesPos] = index;
		}

		inline bool IsValid(Handle<IndexType> handle)
		{
			return m_gens[handle.GetIndex()] == handle.GetGeneration();
		}

		/// <summary>
		///
		/// </summary>
		inline bool IsEmpty() const
		{
			return m_stackIndicesPos == 0;
		}

		/// <summary>
		///
		/// </summary>
		inline void GetSize() const
		{
			return m_stackIndicesPos;
		}

	private:
		void Reserve()
		{
			constexpr size_t alignIdx			 = alignof(IndexType);
			constexpr size_t blockAlign			 = ALIGN > alignIdx ? ALIGN : alignIdx;
			const size_t	 elementBlockSize	 = sizeof(T) * m_size;
			const size_t	 generationBlockSize = sizeof(IndexType) * m_size;
			const size_t	 indexBlockSize		 = sizeof(IndexType) * m_size;

			// Calculate overall size with worst-case padding for each segment.
			const size_t totalSize = elementBlockSize + (blockAlign - 1) + generationBlockSize + (blockAlign - 1) + indexBlockSize;
			m_totalSize			   = ((totalSize + blockAlign - 1) / blockAlign) * blockAlign;
			char* ptr			   = static_cast<char*>(SFG_ALIGNED_MALLOC(blockAlign, m_totalSize));

			// T
			m_raw = reinterpret_cast<T*>(ptr);
			ptr += elementBlockSize;

			// Gens
			std::size_t padding = (alignIdx - (reinterpret_cast<std::uintptr_t>(ptr) % alignIdx)) % alignIdx;
			ptr += padding;
			m_gens = reinterpret_cast<IndexType*>(ptr);
			ptr += generationBlockSize;

			// Indices
			padding = (alignIdx - (reinterpret_cast<std::uintptr_t>(ptr) % alignIdx)) % alignIdx;
			ptr += padding;
			m_stackIndices = reinterpret_cast<IndexType*>(ptr);

			// Default
			for (IndexType i = 0; i < m_size; i++)
			{
				m_gens[i]		  = (IndexType)1;
				m_stackIndices[i] = (IndexType)i;
			}
		}

		void Grow()
		{
			T*		   prev		 = m_raw;
			uint16*	   prevGens	 = m_gens;
			IndexType* prevStack = m_stackIndices;
			uint32	   prevSize	 = m_size;

			const size_t prevTotalSize = m_totalSize;
			m_size *= 2;
			Reserve();

			SFG_MEMCPY(m_raw, prev, prevSize * sizeof(T));
			SFG_MEMCPY(m_gens, prevGens, prevSize * sizeof(IndexType));
			SFG_MEMCPY(m_stackIndices, prevStack, prevSize * sizeof(IndexType));

			SFG_ALIGNED_FREE(prev);
		}

	private:
		T*		   m_raw			 = nullptr;
		IndexType* m_gens			 = nullptr;
		IndexType* m_stackIndices	 = nullptr;
		uint32	   m_stackIndicesPos = 0;
		uint32	   m_size			 = 0;
		size_t	   m_totalSize		 = 0;
	};

	template <typename T, int N> using PoolAllocator16 = PoolAllocator<T, uint16, N, std::alignment_of<T>::value>;

	template <typename T, int N> using PoolAllocator32 = PoolAllocator<T, uint32, N, std::alignment_of<T>::value>;

} // namespace SFG

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

#include "SFG/Memory/BumpAllocator.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/Memory/Memory.hpp"
#include <memory>

namespace SFG
{
	BumpAllocator::BumpAllocator(size_t sz)
	{
		SFG_ASSERT(sz != 0);
		m_size = sz;
		m_raw  = SFG_MALLOC(sz);
	}

	BumpAllocator::~BumpAllocator()
	{
		SFG_FREE(m_raw);
	}

	void* BumpAllocator::Allocate(size_t size, size_t alignment)
	{
		SFG_ASSERT(m_head + size < m_size);

		void*  currentPtr = (void*)((uint8*)m_raw + m_head);
		size_t space	  = m_size - m_head;

		void* alignedPtr = std::align(alignment, size, currentPtr, space);
		if (alignedPtr == nullptr)
			return nullptr;

		m_head = m_size - space + size;
		return alignedPtr;
	}
} // namespace SFG
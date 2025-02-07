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

#include "SFG/Gfx/RenderFrame.hpp"
#include "SFG/Data/Handle.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/Memory/BumpAllocator.hpp"

namespace SFG
{
	void RenderFrame::Initialize(const Definition& definition)
	{
		SFG_ASSERT(m_allocator == nullptr);
		m_allocator		 = new BumpAllocator(definition.bumpAllocatorSize);
		m_commandStreams = m_allocator->Allocate<Handle>(definition.maxCommandStreams);
		m_definition	 = definition;
	}

	void RenderFrame::Shutdown()
	{
		delete m_allocator;
	}

	void RenderFrame::Reset()
	{
		m_allocator->Reset();
		m_commandStreamsCount = 0;
	}

	void RenderFrame::AddCommandStream(const Handle& handle)
	{
		SFG_ASSERT(m_commandStreamsCount < m_definition.maxCommandStreams);
		m_commandStreams[m_commandStreamsCount] = handle;
		m_commandStreamsCount++;
	}
} // namespace SFG

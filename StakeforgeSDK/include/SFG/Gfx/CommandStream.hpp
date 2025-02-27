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
#include "SFG/Data/Handle.hpp"
#include "SFG/StakeforgeAPI.hpp"
#include "SFG/Gfx/Commands/CommandType.hpp"
#include "SFG/Memory/Memory.hpp"

namespace SFG
{
	class BumpAllocator;

	class CommandStream
	{

	public:
		CommandStream()								   = delete;
		CommandStream(const CommandStream&)			   = delete;
		CommandStream& operator=(const CommandStream&) = delete;
		CommandStream(BumpAllocator* alloc, size_t cmdBufferSize);

		/// <summary>
		///
		/// </summary>
		void Allocate(size_t cmdBufferSize);

		/// <summary>
		///
		/// </summary>
		inline uint32 GetCommandsCount() const
		{
			return m_commandsCount;
		}

		/// <summary>
		///
		/// </summary>
		inline uint8* GetCommandsRaw() const
		{
			return m_commandsRaw;
		}

		template <typename T> SFG_API void AddCommand(T&& cmd)
		{
			const CommandType cmdType = T::CMD_TYPE;
			SFG_MEMCPY(m_commandsRaw + m_commandsHeadOffset, &cmdType, sizeof(CommandType));
			m_commandsHeadOffset += sizeof(CommandType);
			const uint32 sz = static_cast<uint32>(sizeof(T));
			SFG_MEMCPY(m_commandsRaw + m_commandsHeadOffset, &sz, sizeof(uint32));
			m_commandsHeadOffset += sizeof(uint32);
			SFG_MEMCPY(m_commandsRaw + m_commandsHeadOffset, &cmd, sizeof(T));
			m_commandsHeadOffset += sizeof(T);
			m_commandsCount++;
		}

	private:
		BumpAllocator* m_allocator			= nullptr;
		uint8*		   m_commandsRaw		= nullptr;
		uint32		   m_commandsCount		= 0;
		size_t		   m_commandsHeadOffset = 0;
	};
}; // namespace SFG

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

namespace SFG
{
	class CommandStream;
	class BumpAllocator;

	class RenderFrame
	{
	public:
		struct Definition
		{
			size_t bumpAllocatorSize = 0;
			size_t maxCommandStreams = 32;
			size_t commandBufferSize = 512;
			size_t maxSubmissions	 = 24;
		};

		struct SubmitDesc
		{
			Handle<uint16> queue;

			CommandStream** streams		 = nullptr;
			uint8			streamsCount = 0;

			Handle<uint16>* waitSemaphores		= nullptr;
			uint64*			waitValues			= nullptr;
			uint8			waitSemaphoresCount = 0;

			Handle<uint16>* signalSemaphores	  = nullptr;
			uint64*			signalValues		  = nullptr;
			uint8			signalSemaphoresCount = 0;
		};

		RenderFrame()							   = default;
		~RenderFrame()							   = default;
		RenderFrame(const RenderFrame& other)	   = delete;
		RenderFrame& operator=(const RenderFrame&) = delete;

		/// <summary>
		///
		/// </summary>
		void Initialize(const Definition& definition);

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		void Reset();

		/// <summary>
		///
		/// </summary>
		/// <param name="handle"></param>
		SFG_API CommandStream& GetCommandStream();

		/// <summary>
		///
		/// </summary>
		SFG_API void Submit(const SubmitDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API BumpAllocator* GetAllocator()
		{
			return m_allocator;
		}

		/// <summary>
		///
		/// </summary>
		inline uint32 GetSubmissionCount() const
		{
			return m_submissionsCount;
		}

		/// <summary>
		///
		/// </summary>
		inline SubmitDesc* GetSubmissions() const
		{
			return m_submissions;
		}

	private:
		Definition	   m_definition			 = {};
		BumpAllocator* m_allocator			 = nullptr;
		CommandStream* m_commandStreams		 = nullptr;
		SubmitDesc*	   m_submissions		 = nullptr;
		uint32		   m_commandStreamsCount = 0;
		uint32		   m_submissionsCount	 = 0;
	};
}; // namespace SFG

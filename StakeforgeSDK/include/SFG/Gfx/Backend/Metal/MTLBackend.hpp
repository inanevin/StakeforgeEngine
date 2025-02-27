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
#include "SFG/Data/String.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/Gfx/Common/GfxConstants.hpp"
#include "SFG/Data/Atomic.hpp"

namespace SFG
{
	class RenderFrame;
	class GfxResources;
	struct CMDBeginRenderPass;

	enum class CommandType;

	class MTLBackend
	{
	private:
		struct CommandData
		{
			void*		  buffer			   = nullptr;
			void*		  currentRenderEncoder = nullptr;
			Vector<void*> renderEncoders	   = {};
			Vector<void*> swapchains		   = {};
		};

		struct PerFrameData
		{
			uint64		   totalSubmits	  = 0;
			Atomic<uint64> reachedSubmits = 0;
		};

	public:
		/// <summary>
		///
		/// </summary>
		void Initialize(String& errString, GfxResources* resources, uint32 maxCommandStreams);

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		void Join();

		/// <summary>
		///
		/// </summary>
		/// <param name="frame"></param>
		void Render(const RenderFrame& frame);

		/// <summary>
		///
		/// </summary>
		void* GetDevice() const
		{
			return m_device;
		}

	private:
		void ResetCommandData(CommandData& data);
		void StartFrame();
		void Present();
		void CMD_BeginRenderPass(CommandData& data, CMDBeginRenderPass* cmd);
		void CMD_EndRenderPass(CommandData& data);

	private:
		PerFrameData		m_pfd[FRAMES_IN_FLIGHT];
		void*				m_device	= nullptr;
		GfxResources*		m_resources = nullptr;
		Vector<CommandData> m_commandData;
		uint8				m_frameIndex = 0;
	};

	typedef MTLBackend GfxBackend;

}; // namespace SFG

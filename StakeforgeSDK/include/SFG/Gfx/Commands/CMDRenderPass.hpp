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

#include "SFG/Data/Handle.hpp"
#include "SFG/Gfx/Common/Viewport.hpp"
#include "SFG/Gfx/Common/Scissors.hpp"
#include "SFG/Gfx/Common/LoadStoreOp.hpp"
#include "SFG/Gfx/Common/ResolveMode.hpp"
#include "SFG/Gfx/Common/TextureFormat.hpp"
#include "SFG/Gfx/Commands/CommandType.hpp"

namespace SFG
{
	struct ColorAttachment
	{
		Handle<uint16> renderTarget;
		Handle<uint16> resolveTarget;
		TextureFormat  format		 = TextureFormat::UNDEFINED;
		LoadOp		   loadOp		 = LoadOp::Load;
		StoreOp		   storeOp		 = StoreOp::Store;
		ResolveMode	   resolveMode	 = ResolveMode::None;
		uint32		   viewIndex	 = 0;
		uint32		   resolveIndex	 = 0;
		float		   clearColor[4] = {0.0f};
	};

	struct DepthAttachment
	{
		Handle<uint16> renderTarget;
		Handle<uint16> resolveTarget;
		TextureFormat  format		= TextureFormat::UNDEFINED;
		LoadOp		   loadOp		= LoadOp::Load;
		StoreOp		   storeOp		= StoreOp::Store;
		ResolveMode	   resolveMode	= ResolveMode::None;
		uint32		   viewIndex	= 0;
		uint32		   resolveIndex = 0;
		float		   clearValue	= 1.0f;
	};

	struct CMDBeginRenderPass
	{
		static constexpr CommandType CMD_TYPE = CommandType::BeginRenderPass;

		Viewport		 viewport			  = {};
		ScissorsRect	 scissors			  = {};
		ColorAttachment* colorAttachments	  = nullptr;
		uint32			 colorAttachmentCount = 0;
		DepthAttachment* depthAttachment	  = nullptr;
	};

	struct CMDEndRenderPass
	{
		static constexpr CommandType CMD_TYPE = CommandType::EndRenderPass;
	};
}; // namespace SFG

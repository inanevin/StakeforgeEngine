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

#ifdef SFG_PLATFORM_WINDOWS
#include "SFG/Gfx/Backend/Vulkan/VkBackend.hpp"
#elif SFG_PLATFORM_OSX
#include "SFG/Gfx/Backend/Metal/MTLBackend.hpp"
#endif

namespace SFG
{
	class RenderFrame;

	class Renderer
	{
	public:
		/// <summary>
		///
		/// </summary>
		void Initialize(String& errString);

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		/// <param name="frame"></param>
		void Render(const RenderFrame& frame);

	private:
#ifdef SFG_PLATFORM_WINDOWS
		VkBackend m_backend;
#elif SFG_PLATFORM_OSX
		MTLBackend m_backend;
#endif
	};
}; // namespace SFG

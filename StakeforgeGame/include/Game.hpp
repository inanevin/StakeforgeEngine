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

#include "SFG/Core/AppDelegate.hpp"
#include "SFG/Data/Handle.hpp"

namespace SFG
{
	class App;
	class Window;

	class Game : public AppDelegate
	{
	public:
		Game(App& app);
		virtual ~Game() = default;

		/// <summary>
		///
		/// </summary>
		virtual void OnWindowEvent(const WindowEvent& ev) override;

		/// <summary>
		///
		/// </summary>
		virtual void OnInitialize() override;

		/// <summary>
		///
		/// </summary>
		virtual void OnShutdown() override;

		/// <summary>
		///
		/// </summary>
		/// <param name="delta"></param>
		virtual void OnTick() override;

		/// <summary>
		///
		/// </summary>
		/// <param name="delta"></param>
		virtual void OnSimulate(double delta) override;

		/// <summary>
		///
		/// </summary>
		/// <param name="frame"></param>
		virtual void OnGenerateFrame(RenderFrame& frame, double interpolation) override;

	private:
		App& m_app;

		Window*		   m_window		  = nullptr;
		Handle<uint16> m_swapchain	  = {};
		Handle<uint16> m_renderTarget = {};
		Handle<uint16> m_shaderGPU	  = {};
	};
} // namespace SFG

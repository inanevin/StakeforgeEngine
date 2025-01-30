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

#include "SFG/Data/String.hpp"
#include "SFG/Data/Atomic.hpp"
#include <thread>

namespace SFG
{

	class Window;
	class AppDelegate;

	class App
	{
	public:
		App() = delete;
		App(AppDelegate* delegate) : m_appDelegate(delegate) {};
		App(App& other) = delete;

		/// <summary>
		///
		/// </summary>
		/// <param name="errString"></param>
		/// <returns></returns>
		bool Initialize(String& errString);

		/// <summary>
		///
		/// </summary>
		void Tick();

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		void RequestClose();

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline Atomic<bool>& GetShouldClose()
		{
			return m_shouldClose;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline AppDelegate* GetDelegate() const
		{
			return m_appDelegate;
		}

	private:
		void UpdateLoop();
		void RenderLoop();

	private:
		Window*		 m_mainWindow  = nullptr;
		AppDelegate* m_appDelegate = nullptr;

		std::thread	 m_updateThread;
		std::thread	 m_renderThread;
		Atomic<bool> m_shouldClose = false;
	};
} // namespace SFG

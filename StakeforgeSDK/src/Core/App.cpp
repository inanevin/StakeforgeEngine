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

#include "SFG/Core/App.hpp"
#include "SFG/Core/AppDelegate.hpp"
#include "SFG/Platform/Time.hpp"
#include "SFG/Platform/Process.hpp"
#include "SFG/Platform/Window.hpp"

namespace SFG
{
	bool App::Initialize(String& errString)
	{
		if (m_appDelegate == nullptr)
		{
			errString = "Application Delegate was not set!";
			return false;
		}

		Time::Initialize();

		m_mainWindow = Window::Create(0, Vector2i::Zero, Vector2ui(500, 500), "SFG", WindowStyle::ApplicationWindow);
		m_mainWindow->CenterToMonitor();

		m_updateThread = std::thread(&App::UpdateLoop, this);
		m_renderThread = std::thread(&App::RenderLoop, this);

		return true;
	}

	void App::Tick()
	{
		/*
			Pump OS messages at 1000Hz.
			Accumulate for consistency.
			Sleep for chilling CPU.
		*/

		Process::PumpOSMessages();

		if (m_mainWindow->GetCloseRequested())
		{
			RequestClose();
			return;
		}

		constexpr int64 TARGET_INTERVAL_US = 1000;
		static int64	accumulator		   = 0;
		static int64	previous		   = Time::GetCPUMicroseconds();
		const int64		current			   = Time::GetCPUMicroseconds();
		const int64		deltaMicroseconds  = current - previous;
		previous						   = current;

		accumulator += deltaMicroseconds;
		if (accumulator < TARGET_INTERVAL_US)
		{
			const int64 sleepTimeUs = TARGET_INTERVAL_US - accumulator;
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTimeUs));
		}
		accumulator -= TARGET_INTERVAL_US;
	}

	void App::UpdateLoop()
	{
		const int64 targetFrameRate			 = static_cast<int64>(m_appDelegate->GetTargetFrameRate());
		const int64 FIXED_UPDATE_INTERVAL_US = (int64)1000000 / targetFrameRate;
		int64		accumulator				 = 0;
		int64		previousTime			 = Time::GetCPUMicroseconds();

		while (!m_shouldClose.load(std::memory_order_acquire))
		{
			const int64 currentTime		  = Time::GetCPUMicroseconds();
			const int64 deltaMicroseconds = currentTime - previousTime;
			previousTime				  = currentTime;
			accumulator += deltaMicroseconds;

			while (accumulator >= FIXED_UPDATE_INTERVAL_US)
			{
				accumulator -= FIXED_UPDATE_INTERVAL_US;

				/*
					Try popping all the events produced by the main thread (PumpOSMessages)
					Pass them down to application delegate.
				*/
				MouseEvent mouseEvent = {};
				while (m_mainWindow->PopMouseEvent(mouseEvent))
					m_appDelegate->OnMouse(mouseEvent);

				KeyEvent keyEvent = {};
				while (m_mainWindow->PopKeyEvent(keyEvent))
					m_appDelegate->OnKey(keyEvent);

				MouseWheelEvent mouseWheelEvent = {};
				while (m_mainWindow->PopMouseWheelEvent(mouseWheelEvent))
					m_appDelegate->OnMouseWheel(mouseWheelEvent);

				MouseDeltaEvent mouseDeltaEvent = {};
				while (m_mainWindow->PopMouseDeltaEvent(mouseDeltaEvent))
					m_appDelegate->OnMouseDelta(mouseDeltaEvent);

				const double delta = FIXED_UPDATE_INTERVAL_US * 1e-6;
				m_appDelegate->OnTick(delta);
			}

			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

	void App::RenderLoop()
	{
	}

	void App::Shutdown()
	{
		if (m_updateThread.joinable())
			m_updateThread.join();

		if (m_renderThread.joinable())
			m_renderThread.join();

		Window::Destroy(m_mainWindow);
		m_mainWindow = nullptr;

		Time::Shutdown();
	}

	void App::RequestClose()
	{
		m_shouldClose.store(true, std::memory_order_release);
	}

} // namespace SFG

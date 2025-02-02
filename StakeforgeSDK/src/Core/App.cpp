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
#include "SFG/IO/FileSystem.hpp"
#include "SFG/Platform/Time.hpp"
#include "SFG/Platform/Process.hpp"
#include "SFG/Platform/Window.hpp"

namespace SFG
{

	App::App(String& errString)
	{
		Time::Initialize();

		// m_renderThread = std::thread(&App::RenderLoop, this);
	}

	App::~App()
	{
		// if (m_renderThread.joinable())
		//	m_renderThread.join();

		for (Window* window : m_windows)
			Window::Destroy(window);
		m_windows.clear();

		Time::Shutdown();
	}

	void App::Tick()
	{
		/*
			Pump OS messages and update app at fixed rate.
			No accumulation for OS messages, but accumulate the app.
			Sleep for chilling CPU if settings permit.
		*/

		const int64_t INPUT_INTERVAL_US = m_settings.inputUpdateRate == 0 ? 0 : (int64_t)1000000 / static_cast<int64_t>(m_settings.inputUpdateRate);
		const int64_t APP_INTERVAL_US	= m_settings.appUpdateRate == 0 ? 0 : (int64_t)1000000 / static_cast<int64_t>(m_settings.appUpdateRate);
		int64_t		  previousTime		= Time::GetCPUMicroseconds();
		int64_t		  inputAccumulator	= INPUT_INTERVAL_US + 1;
		int64_t		  appAccumulator	= INPUT_INTERVAL_US + 1;

		while (!m_shouldClose.load(std::memory_order_acquire))
		{
			const int64_t currentTime		= Time::GetCPUMicroseconds();
			const int64_t deltaMicroseconds = currentTime - previousTime;
			previousTime					= currentTime;

			inputAccumulator += deltaMicroseconds;

			if (inputAccumulator > INPUT_INTERVAL_US)
			{
				Process::PumpOSMessages();
				inputAccumulator = 0;
			}

			/*
				Go through windows, destroy the ones marked dead.
			*/
			for (Vector<Window*>::iterator it = m_windows.begin(); it != m_windows.end();)
			{
				Window* window = *it;

				if (window->GetCloseRequested())
				{
					const uint32_t id = window->GetID();

					it = UtilVector::Remove(m_windows, window);
					Window::Destroy(window);

					// main window
					if (id == 0)
						RequestClose();

					continue;
				}

				if (window->GetIsSizeDirty())
				{
				}

				window->Consume();

				++it;
			}

			/*
				Main app loop w/ accumulator, sleep/yield if throttle requested afterwards
			*/
			appAccumulator += deltaMicroseconds;
			while (appAccumulator > APP_INTERVAL_US)
			{
				appAccumulator -= APP_INTERVAL_US;

				if (!m_settings.delegate)
					continue;

				/*
					Collect all window events buffered so far & pass them to delegate.
				*/
				static Vector<WindowEvent> bufferedEvents = {};
				bufferedEvents.resize(0);

				for (Window* window : m_windows)
				{
					WindowEvent ev = {};
					while (window->PopEvent(ev))
						bufferedEvents.push_back(ev);
				}

				for (const WindowEvent& ev : bufferedEvents)
					m_settings.delegate->OnWindowEvent(ev);

				const double delta = APP_INTERVAL_US * 1e-6;
				m_settings.delegate->OnTick(delta);
			}

			if (m_settings.throttleCPU)
				Time::Sleep(1);
		}
	}

	void App::RenderLoop()
	{
	}

	void App::RequestClose()
	{
		m_shouldClose.store(true, std::memory_order_release);
	}

	Window* App::CreateAppWindow(uint32_t id, const char* title, const Vector2i& pos, const Vector2ui& size, WindowStyle style)
	{
		Window* window = Window::Create(id, pos, size, title, style);

		if (window)
			m_windows.push_back(window);

		return window;
	}

	Window* App::GetWindow(uint32_t id)
	{
		Window* retVal = nullptr;
		UtilVector::Find(m_windows, retVal, [id](Window* w) -> bool { return w->GetID() == id; });
		return retVal;
	}

	void App::DestroyAppWindow(Window* window)
	{
		window->Close();
	}

} // namespace SFG

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
#include "SFG/Gfx/RenderFrame.hpp"
#include "SFG/IO/Log.hpp"

namespace SFG
{
	App::App(String& errString)
	{
		Time::Initialize();
		m_renderer.Initialize(errString);

		m_renderFrames[0].Initialize({
			.bumpAllocatorSize = 1024 * 1024,
			.maxCommandStreams = 32,
		});

		m_renderFrames[1].Initialize({
			.bumpAllocatorSize = 1024 * 1024,
			.maxCommandStreams = 32,
		});

		if (!errString.empty())
			m_shouldClose.store(true);

		m_currentRenderFrameIndex.store(0);
		m_updateRenderFrameIndex = 1;
		m_renderThread			 = std::thread(&App::RenderLoop, this);
	}

	App::~App()
	{
		if (m_renderThread.joinable())
			m_renderThread.join();

		m_renderFrames[0].Shutdown();
		m_renderFrames[1].Shutdown();
		m_renderer.Shutdown();

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

		const int64 FIXED_INTERVAL_US = m_settings.fixedUpdateRate == 0 ? 0 : (int64)1000000 / static_cast<int64>(m_settings.fixedUpdateRate);
		int64		previousTime	  = Time::GetCPUMicroseconds();
		int64		accumulator		  = FIXED_INTERVAL_US;

		while (!m_shouldClose.load(std::memory_order_acquire))
		{
			const int64 currentTime		  = Time::GetCPUMicroseconds();
			const int64 deltaMicroseconds = currentTime - previousTime;
			previousTime				  = currentTime;

			Process::PumpOSMessages();

			/*
				Go through windows, destroy the ones marked dead.
			*/
			for (Vector<Window*>::iterator it = m_windows.begin(); it != m_windows.end();)
			{
				Window* window = *it;

				if (window->GetCloseRequested())
				{
					const uint32 id = window->GetID();

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

			if (!m_settings.delegate)
			{
				if (m_settings.throttleCPU)
					Time::Sleep(1);

				continue;
			}

			/*
				Main app loop w/ accumulator.
				OnTick() is for high frequency actions, called as fast as the loop's running.
				OnSimulate() is discrete timestep simulation.
				Sleep/yield if throttle requested.
			*/

			m_settings.delegate->OnTick();

			accumulator += deltaMicroseconds;

			/*
					Collect all window events buffered so far & pass them to delegate.
			*/
			static Vector<WindowEvent> bufferedEvents = {};
			bufferedEvents.resize(0);

			// Time to collect events.
			if (accumulator >= FIXED_INTERVAL_US)
			{
				for (Window* window : m_windows)
				{
					WindowEvent ev = {};
					while (window->PopEvent(ev))
						bufferedEvents.push_back(ev);
				}

				for (const WindowEvent& ev : bufferedEvents)
					m_settings.delegate->OnWindowEvent(ev);
			}

			while (accumulator >= FIXED_INTERVAL_US)
			{
				const double delta = FIXED_INTERVAL_US * 1e-6;
				accumulator -= FIXED_INTERVAL_US;
				m_settings.delegate->OnSimulate(delta);
			}

			/* Generate a new render frame & signal render thread. */
			const double interpolation = static_cast<double>(accumulator) / FIXED_INTERVAL_US;
			RenderFrame& frameToWrite  = m_renderFrames[m_updateRenderFrameIndex];
			frameToWrite.Reset();
			m_settings.delegate->OnGenerateFrame(frameToWrite, interpolation);
			m_currentRenderFrameIndex.store(m_updateRenderFrameIndex, std::memory_order_release);
			m_updateRenderFrameIndex = (m_updateRenderFrameIndex + 1) % 2;
			m_frameAvailableSemaphore.release();

			if (m_settings.throttleCPU)
				Time::Sleep(1);
		}
	}

	void App::RenderLoop()
	{
		while (!m_shouldClose.load(std::memory_order_acquire))
		{
			m_frameAvailableSemaphore.acquire();
			const uint32	   index	   = m_currentRenderFrameIndex.load(std::memory_order_acquire);
			const RenderFrame& frameToRead = m_renderFrames[index];
			m_renderer.Render(frameToRead);

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void App::RequestClose()
	{
		m_shouldClose.store(true, std::memory_order_release);
	}

	Window* App::CreateAppWindow(uint32 id, const char* title, const Vector2i& pos, const Vector2ui& size, WindowStyle style)
	{
		Window* window = Window::Create(id, pos, size, title, style);

		if (window)
			m_windows.push_back(window);

		return window;
	}

	Window* App::GetWindow(uint32 id)
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

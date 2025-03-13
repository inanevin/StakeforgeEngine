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
#include "SFG/Data/VectorUtil.hpp"
#include "SFG/IO/FileSystem.hpp"
#include "SFG/IO/Assert.hpp"
#include "SFG/Platform/Time.hpp"
#include "SFG/Platform/Process.hpp"
#include "SFG/Platform/Window.hpp"
#include "SFG/Gfx/RenderFrame.hpp"
#include "SFG/Resources/Shader.hpp"
#include "SFG/IO/Log.hpp"

namespace SFG
{

	void App::Initialize(String& errString)
	{
		Time::Initialize();

		m_gfxBackend.Initialize(errString, &m_gfxResources, m_settings.maxCommandStreamsPerFrame);
		m_gfxResources.Initialize(&m_gfxBackend);
		m_resourceManager.Initialize();

		m_renderFrames[0].Initialize({
			.bumpAllocatorSize = m_settings.bumpAllocatorSizePerFrame,
			.maxCommandStreams = m_settings.maxCommandStreamsPerFrame,
			.commandBufferSize = m_settings.commandStreamSize,
			.maxSubmissions	   = m_settings.maxSubmissionsPerFrame,
		});

		m_renderFrames[1].Initialize({
			.bumpAllocatorSize = m_settings.bumpAllocatorSizePerFrame,
			.maxCommandStreams = m_settings.maxCommandStreamsPerFrame,
			.commandBufferSize = m_settings.commandStreamSize,
			.maxSubmissions	   = m_settings.maxSubmissionsPerFrame,
		});

		if (!Shader::InitializeSlang())
		{
			errString = "Failed initializing slang!";
		}

		if (!errString.empty())
			m_shouldClose.store(true);

		m_renderJoined.store(true);
		StartRender();

		if (m_settings.delegate)
			m_settings.delegate->OnInitialize();
	}

	void App::Shutdown()
	{
		if (m_settings.delegate)
			m_settings.delegate->OnShutdown();

		if (m_renderThread.joinable())
			m_renderThread.join();

		m_resourceManager.Shutdown();
		m_renderFrames[0].Shutdown();
		m_renderFrames[1].Shutdown();
		m_gfxBackend.Shutdown();
		m_gfxResources.Shutdown();

		SFG_ASSERT(m_windows.empty());
		Time::Shutdown();
		Shader::ShutdownSlang();
	}

	void App::JoinRender()
	{
		if (m_renderJoined.load())
			return;

		m_renderJoined.store(true, std::memory_order_release);
		m_frameAvailableSemaphore.release();

		if (m_renderThread.joinable())
			m_renderThread.join();

		m_gfxBackend.Join();
	}

	void App::StartRender()
	{
		if (!m_renderJoined.load())
			return;

		m_renderJoined.store(false, std::memory_order_release);
		m_renderFrames[0].Reset();
		m_renderFrames[1].Reset();
		m_currentRenderFrameIndex.store(0);
		m_updateRenderFrameIndex = 1;
		m_renderThread			 = std::thread(&App::RenderLoop, this);
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

			const bool wasJoined = m_renderJoined;

			for (Vector<Window*>::iterator it = m_windows.begin(); it != m_windows.end();)
			{
				Window* window = *it;

				if (window->GetCloseRequested() && m_settings.delegate)
				{
					if (!m_renderJoined)
						JoinRender();

					const WindowEvent ev = {
						.type	= WindowEventType::PreDestroy,
						.window = window,
					};
					m_settings.delegate->OnWindowEvent(ev);

					if (window->GetID() == 0)
						RequestClose();

					it = UtilVector::Remove(m_windows, window);
					Window::Destroy(window);
					continue;
				}

				if (window->GetIsSizeDirty() && m_settings.delegate)
				{
					if (!m_renderJoined)
						JoinRender();

					const WindowEvent ev = {
						.type	= WindowEventType::Resized,
						.window = window,
					};

					m_settings.delegate->OnWindowEvent(ev);
				}

				window->Consume();
				++it;
			}

			// Called delegate, kick off again.
			if (m_renderJoined && !wasJoined)
				StartRender();

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

			uint32 accCount = 0;
			while (accumulator >= FIXED_INTERVAL_US)
			{
				const double delta = FIXED_INTERVAL_US * 1e-6;
				accumulator -= FIXED_INTERVAL_US;
				m_settings.delegate->OnSimulate(delta);

				accCount++;

				if (accCount >= m_settings.maxAccumulatedUpdates)
				{
					accumulator = 0.0f;
					break;
				}
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
		while (!m_shouldClose.load(std::memory_order_acquire) && !m_renderJoined.load(std::memory_order_acquire))
		{
			m_frameAvailableSemaphore.acquire();
			const uint32	   index	   = m_currentRenderFrameIndex.load(std::memory_order_acquire);
			const RenderFrame& frameToRead = m_renderFrames[index];
			m_gfxBackend.Render(frameToRead);
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
		UtilVector::Remove(m_windows, window);
		Window::Destroy(window);
	}

} // namespace SFG

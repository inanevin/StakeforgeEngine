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
#include "SFG/StakeforgeAPI.hpp"
#include "SFG/Data/String.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/Data/Atomic.hpp"
#include "SFG/Data/Semaphore.hpp"
#include "SFG/Gfx/RenderFrame.hpp"
#include "SFG/Gfx/GfxResources.hpp"
#include "SFG/Resources/ResourceManager.hpp"

#ifdef SFG_PLATFORM_WINDOWS
#include "SFG/Gfx/Backend/Vulkan/VulkanBackend.hpp"
#elif SFG_PLATFORM_OSX
#include "SFG/Gfx/Backend/Metal/MTLBackend.hpp"
#endif

#include <thread>

namespace SFG
{
	class Window;
	class Plugin;
	class Vector2ui;
	class Vector2i;
	class AppDelegate;
	enum class WindowStyle;

	class SFG_API App
	{
	public:
		struct Settings
		{
			AppDelegate* delegate				   = nullptr;
			uint32		 fixedUpdateRate		   = 60;
			uint32		 maxAccumulatedUpdates	   = 4;
			uint32		 maxCommandStreamsPerFrame = 8;
			uint32		 bumpAllocatorSizePerFrame = 1024 * 1024;
			uint32		 commandStreamSize		   = 1024;
			uint32		 maxSubmissionsPerFrame	   = 24;
			bool		 throttleCPU			   = false;
		};

		App()							 = default;
		App(const App& other)			 = delete;
		App& operator=(const App& other) = delete;
		~App()							 = default;

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
		void Tick();

		/// <summary>
		///
		/// </summary>
		void JoinRender();

		/// <summary>
		///
		/// </summary>
		void StartRender();

		/// <summary>
		///
		/// </summary>
		void RequestClose();

		/// <summary>
		///
		/// </summary>
		/// <param name="title"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		Window* CreateAppWindow(uint32 id, const char* title, const Vector2i& pos, const Vector2ui& size, WindowStyle style);

		/// <summary>
		///
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		Window* GetWindow(uint32 id);

		/// <summary>
		///
		/// </summary>
		/// <param name="window"></param>
		void DestroyAppWindow(Window* window);

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
		inline Settings& GetAppSettings()
		{
			return m_settings;
		}

		/// <summary>
		///
		/// </summary>
		inline GfxResources& GetGfxResources()
		{
			return m_gfxResources;
		}

		/// <summary>
		///
		/// </summary>
		inline ResourceManager& GetResourceManager()
		{
			return m_resourceManager;
		}

	private:
		void RenderLoop();

	private:
		GfxBackend		m_gfxBackend;
		GfxResources	m_gfxResources;
		ResourceManager m_resourceManager;
		RenderFrame		m_renderFrames[2];
		Vector<Window*> m_windows;
		Settings		m_settings = {};
		std::thread		m_renderThread;
		BinarySemaphore m_frameAvailableSemaphore{0};
		uint32			m_updateRenderFrameIndex  = 0;
		Atomic<uint32>	m_currentRenderFrameIndex = 0;
		Atomic<bool>	m_shouldClose			  = false;
		Atomic<bool>	m_renderJoined			  = false;
	};
} // namespace SFG

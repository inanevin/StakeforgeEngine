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
#include "SFG/Data/Atomic.hpp"
#include "SFG/Data/Queue.hpp"
#include "SFG/Math/Vector2ui.hpp"
#include "SFG/Math/Vector2i.hpp"
#include "SFG/Math/Vector2.hpp"

#include "SFG/Platform/WindowEvent.hpp"
#include "SFG/Platform/WindowStyle.hpp"
#include "SFG/Platform/MonitorInfo.hpp"

#ifdef SFG_PLATFORM_WINDOWS
struct HWND__;
#endif

namespace SFG
{
#define INPUT_EVENT_BUFFER_SIZE 100

	class SFG_API Window
	{
	public:
		Window(Window& window) = delete;

#ifdef SFG_PLATFORM_WINDOWS
		static __int64 WndProc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
#endif

	private:
		friend class App;

		Window()  = default;
		~Window() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="id"></param>
		/// <param name="pos"></param>
		/// <param name="size"></param>
		/// <param name="title"></param>
		/// <param name="style"></param>
		/// <returns></returns>
		static Window* Create(uint32 id, const Vector2i& pos, const Vector2ui& size, const char* title, WindowStyle style);

		/// <summary>
		///
		/// </summary>
		/// <param name="window"></param>
		static void Destroy(Window* window);

	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector2i& pos);

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		void SetSize(const Vector2ui& size);

		/// <summary>
		///
		/// </summary>
		/// <param name="style"></param>
		void SetStyle(WindowStyle style);

		/// <summary>
		///
		/// </summary>
		void BringToFront();

		/// <summary>
		///
		/// </summary>
		inline void Close()
		{
			m_closeRequested = true;
		}

		/// <summary>
		///
		/// </summary>
		inline void CenterToMonitor()
		{
			const uint32 centerX = m_monitorInfo.position.x + static_cast<int32>(m_monitorInfo.fullSize.x) / 2;
			const uint32 centerY = m_monitorInfo.position.y + static_cast<int32>(m_monitorInfo.fullSize.y) / 2;
			SetPosition(Vector2i(centerX - static_cast<int32>(m_trueSize.x) / 2, centerY - static_cast<int32>(m_trueSize.y) / 2));
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const MonitorInfo& GetMonitor() const
		{
			return m_monitorInfo;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint32 GetID() const
		{
			return m_id;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector2i& GetPosition() const
		{
			return m_position;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector2ui& GetSize() const
		{
			return m_size;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const Vector2i& GetMousePosition() const
		{
			return m_mousePosition;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline void* GetOSHandle() const
		{
			return m_osHandle;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline void* GetWindowHandle() const
		{
			return m_windowHandle;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline bool GetHasFocus() const
		{
			return m_hasFocus;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline bool GetCloseRequested() const
		{
			return m_closeRequested;
		}

		inline bool GetIsSizeDirty() const
		{
			return m_sizeDirty;
		}

		/// <summary>
		///
		/// </summary>
		inline void Consume()
		{
			m_sizeDirty		 = false;
			m_closeRequested = false;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="hf"></param>
		inline void SetHighFrequencyInputMode(bool hf)
		{
			m_highFrequencyInputMode = hf;
		}

	private:
		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		inline void AddEvent(const WindowEvent& ev)
		{
			if (m_windowEvents.size() == BUFFER_SIZE)
				m_windowEvents.pop();

			m_windowEvents.push(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		/// <returns></returns>
		inline bool PopEvent(WindowEvent& ev)
		{
			if (m_windowEvents.empty())
				return false;

			ev = m_windowEvents.front();
			m_windowEvents.pop();
			return true;
		}

	private:
		static constexpr int BUFFER_SIZE = 256;

		Queue<WindowEvent> m_windowEvents = Queue<WindowEvent>();

		MonitorInfo m_monitorInfo			 = {};
		Vector2i	m_position				 = Vector2i::Zero;
		Vector2ui	m_size					 = Vector2ui::Zero;
		Vector2ui	m_trueSize				 = Vector2ui::Zero;
		Vector2i	m_mousePosition			 = Vector2i::Zero;
		void*		m_osHandle				 = nullptr;
		void*		m_windowHandle			 = nullptr;
		uint32		m_id					 = 0;
		WindowStyle m_style					 = WindowStyle::ApplicationWindow;
		bool		m_sizeDirty				 = false;
		bool		m_hasFocus				 = false;
		bool		m_closeRequested		 = false;
		bool		m_highFrequencyInputMode = false;
	};
} // namespace SFG

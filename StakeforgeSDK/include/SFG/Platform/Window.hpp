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
#include "SFG/Data/Atomic.hpp"
#include "SFG/Math/Vector2ui.hpp"
#include "SFG/Math/Vector2i.hpp"
#include "SFG/Math/Vector2.hpp"

#include "SFG/Platform/KeyEvent.hpp"
#include "SFG/Platform/MouseEvent.hpp"
#include "SFG/Platform/MouseWheelEvent.hpp"
#include "SFG/Platform/MouseDeltaEvent.hpp"
#include "SFG/Platform/MonitorInfo.hpp"

#include <SFG/Vendor/readerwriterqueue/readerwriterqueue.h>

#ifdef SFG_PLATFORM_WINDOWS
struct HWND__;
#endif

namespace SFG
{
	enum class WindowStyle
	{
		ApplicationWindow,
		Borderless,
	};

	template <typename T> using InputQueue = moodycamel::BlockingReaderWriterQueue<T>;

#define INPUT_EVENT_BUFFER_SIZE 100

	class Window
	{
	public:
		Window()  = default;
		~Window() = default;

#ifdef SFG_PLATFORM_WINDOWS
		static LRESULT WndProc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
#endif

		/// <summary>
		///
		/// </summary>
		/// <param name="id"></param>
		/// <param name="pos"></param>
		/// <param name="size"></param>
		/// <param name="title"></param>
		/// <param name="style"></param>
		/// <returns></returns>
		static Window* Create(uint32 id, const Vector2i& pos, const Vector2ui& size, const String& title, WindowStyle style);

		/// <summary>
		///
		/// </summary>
		/// <param name="window"></param>
		static void Destroy(Window* window);

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
		inline void CenterToMonitor()
		{
			const uint32 centerX = m_monitorInfo.position.x + static_cast<int32>(m_monitorInfo.fullSize.x) / 2;
			const uint32 centerY = m_monitorInfo.position.y + static_cast<int32>(m_monitorInfo.fullSize.y) / 2;
			SetPosition(Vector2i(centerX - static_cast<int32>(m_trueSize.x) / 2, centerY - static_cast<int32>(m_trueSize.y) / 2));
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		inline void AddMouseEvent(const MouseEvent& ev)
		{
			m_mouseEvents.try_enqueue(ev);
		}

		inline bool PopMouseEvent(MouseEvent& ev)
		{
			return m_mouseEvents.try_dequeue(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		inline void AddMouseWheelEvent(const MouseWheelEvent& ev)
		{
			m_mouseWheelEvents.try_enqueue(ev);
		}

		inline bool PopMouseWheelEvent(MouseWheelEvent& ev)
		{
			return m_mouseWheelEvents.try_dequeue(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		inline void AddMouseDeltaEvent(const MouseDeltaEvent& ev)
		{
			m_mouseDeltaEvents.try_enqueue(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		/// <returns></returns>
		inline bool PopMouseDeltaEvent(MouseDeltaEvent& ev)
		{
			return m_mouseDeltaEvents.try_dequeue(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		inline void AddKeyEvent(const KeyEvent& ev)
		{
			m_keyEvents.try_enqueue(ev);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ev"></param>
		/// <returns></returns>
		inline bool PopKeyEvent(KeyEvent& ev)
		{
			return m_keyEvents.try_dequeue(ev);
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
		inline const Vector2ui& GetMousePosition() const
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

		/// <summary>
		///
		/// </summary>
		inline void Consume()
		{
			m_sizeDirty		 = false;
			m_closeRequested = false;
		}


	private:
		static constexpr int BUFFER_SIZE	 = 256;
		static constexpr int BUFFER_SIZE_BIG = 512;

		InputQueue<KeyEvent>		m_keyEvents		   = InputQueue<KeyEvent>(BUFFER_SIZE);
		InputQueue<MouseEvent>		m_mouseEvents	   = InputQueue<MouseEvent>(BUFFER_SIZE);
		InputQueue<MouseDeltaEvent> m_mouseDeltaEvents = InputQueue<MouseDeltaEvent>(BUFFER_SIZE_BIG);
		InputQueue<MouseWheelEvent> m_mouseWheelEvents = InputQueue<MouseWheelEvent>(BUFFER_SIZE);

		MonitorInfo m_monitorInfo	 = {};
		Vector2i	m_position		 = Vector2i::Zero;
		Vector2ui	m_size			 = Vector2ui::Zero;
		Vector2ui	m_trueSize		 = Vector2ui::Zero;
		Vector2ui	m_mousePosition	 = Vector2ui::Zero;
		void*		m_osHandle		 = nullptr;
		void*		m_windowHandle	 = nullptr;
		uint32		m_id			 = 0;
		WindowStyle m_style			 = WindowStyle::ApplicationWindow;
		bool		m_sizeDirty		 = false;
		bool		m_hasFocus		 = false;
		bool		m_closeRequested = false;
	};
} // namespace SFG

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

#include "SFG/Platform/Window.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Math/Math.hpp"
#include "SFG/Platform/InputMappings.hpp"
#include "SFG/Platform/InputAction.hpp"

#include <windowsx.h>
#include <hidusage.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Shcore.lib")

namespace SFG
{
	namespace
	{
		auto composition_enabled() -> bool
		{
			BOOL composition_enabled = FALSE;
			bool success			 = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
			return composition_enabled && success;
		}

		uint32_t GetStyle(WindowStyle s)
		{
			if (s == WindowStyle::ApplicationWindow)
				return static_cast<uint32_t>(WS_OVERLAPPEDWINDOW);
			else
			{
				DWORD style = 0;
				if (composition_enabled())
					style = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
				else
					style = WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

				return style;
			}
		}

		MonitorInfo FetchMonitorInfo(HMONITOR monitor)
		{
			MonitorInfo info;

			MONITORINFOEX monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(monitor, &monitorInfo);

			UINT	dpiX, dpiY;
			HRESULT temp2  = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
			info.fullSize  = {static_cast<uint32_t>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left), static_cast<uint32_t>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)};
			info.workSize  = {static_cast<uint32_t>(monitorInfo.rcWork.right - monitorInfo.rcWork.left), static_cast<uint32_t>(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top)};
			info.position  = {static_cast<int32_t>(monitorInfo.rcWork.left), static_cast<int32_t>(monitorInfo.rcWork.top)};
			info.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
			info.dpi	   = dpiX;
			info.dpiScale  = static_cast<float>(dpiX) / 96.0f;
			return info;
		}
	} // namespace

	LRESULT __stdcall Window::WndProc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
	{
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
		case WM_DPICHANGED:
		case WM_DISPLAYCHANGE: {
			window->m_monitorInfo = FetchMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));
			break;
		}
		case WM_CLOSE: {
			window->Close();
			return 0;
		}
		case WM_KILLFOCUS: {
			window->m_hasFocus = false;
			return 0;
		}
		case WM_SETFOCUS: {
			window->m_hasFocus = true;
			return 0;
		}
		case WM_MOVE: {
			const int32_t x	   = static_cast<int32_t>((short)LOWORD(lParam));
			const int32_t y	   = static_cast<int32_t>((short)HIWORD(lParam));
			window->m_position = Vector2i(x, y);
			return 0;
		}
		case WM_SIZE: {

			UINT width	= LOWORD(lParam);
			UINT height = HIWORD(lParam);

			RECT rect;
			GetWindowRect(hwnd, &rect);

			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			window->m_size		= Vector2ui(width, height);
			window->m_trueSize	= Vector2ui(width, height);
			window->m_sizeDirty = true;

			if (window->m_style == WindowStyle::ApplicationWindow)
			{
				window->m_trueSize.x = rect.right - rect.left;
				window->m_trueSize.y = rect.bottom - rect.top;
			}
			break;
		}
		case WM_INPUT: {

			if (!window->m_highFrequencyInputMode)
				return 0;

			UINT		dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				// Handle keyboard input
				const USHORT key	   = raw->data.keyboard.VKey;
				USHORT		 scanCode  = raw->data.keyboard.MakeCode;
				const bool	 isPress   = raw->data.keyboard.Flags & RI_KEY_MAKE;
				const bool	 isRelease = raw->data.keyboard.Flags & RI_KEY_BREAK;

				const WindowEvent ev = {
					.window			 = window,
					.type			 = WindowEventType::Key,
					.action			 = isRelease ? InputAction::Released : InputAction::Pressed,
					.value			 = Vector2i(static_cast<int32_t>(scanCode), 0),
					.isHighFrequency = true,
				};
				window->AddEvent(ev);
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				USHORT mouseFlags = raw->data.mouse.usButtonFlags;
				POINT  cursorPos;
				GetCursorPos(&cursorPos);
				const Vector2i relative = Vector2i(static_cast<int32_t>(cursorPos.x), static_cast<int32_t>(cursorPos.y)) - window->m_position;

				WindowEvent ev = {
					.window			 = window,
					.type			 = WindowEventType::MouseButton,
					.value			 = relative,
					.isHighFrequency = true,
				};

				bool WindowEventExists = false;

				if (mouseFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
				{
					ev.button		  = InputCode::Mouse0;
					ev.action		  = InputAction::Pressed;
					WindowEventExists = true;
				}
				if (mouseFlags & RI_MOUSE_LEFT_BUTTON_UP)
				{
					ev.button		  = InputCode::Mouse0;
					ev.action		  = InputAction::Released;
					WindowEventExists = true;
				}
				if (mouseFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
				{
					ev.button		  = InputCode::Mouse1;
					ev.action		  = InputAction::Pressed;
					WindowEventExists = true;
				}
				if (mouseFlags & RI_MOUSE_RIGHT_BUTTON_UP)
				{
					ev.button		  = InputCode::Mouse1;
					ev.action		  = InputAction::Released;
					WindowEventExists = true;
				}
				if (mouseFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				{
					ev.button		  = InputCode::Mouse2;
					ev.action		  = InputAction::Pressed;
					WindowEventExists = true;
				}
				if (mouseFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
				{
					ev.button		  = InputCode::Mouse2;
					ev.action		  = InputAction::Released;
					WindowEventExists = true;
				}

				if (WindowEventExists)
					window->AddEvent(ev);
				else if (mouseFlags & RI_MOUSE_WHEEL)
				{
					const uint16_t wheelDelta = (uint16_t)raw->data.mouse.usButtonData;
					const short	   wheel	  = (short)raw->data.mouse.usButtonData / (short)WHEEL_DELTA;

					const WindowEvent mwe = {
						.window			 = window,
						.type			 = WindowEventType::MouseWheel,
						.value			 = Vector2i(0, wheel),
						.isHighFrequency = true,
					};
					window->AddEvent(mwe);
				}
				else
				{
					const int32_t xPosRelative = raw->data.mouse.lLastX;
					const int32_t yPosRelative = raw->data.mouse.lLastY;

					const WindowEvent mdEvent = {
						.window			 = window,
						.type			 = WindowEventType::MouseDelta,
						.value			 = Vector2i(xPosRelative, yPosRelative),
						.isHighFrequency = true,
					};

					window->AddEvent(mdEvent);
				}
			}
			return 0;
		}
		case WM_KEYDOWN: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const WORD keyFlags	  = HIWORD(lParam);
			const WORD scanCode	  = LOBYTE(keyFlags);
			const int  extended	  = (lParam & 0x01000000) != 0;
			const bool isRepeated = (HIWORD(lParam) & KF_REPEAT) != 0;
			uint32_t   key		  = static_cast<uint32_t>(wParam);

			if (wParam == VK_SHIFT)
				key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
			else if (wParam == VK_CONTROL)
				key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

			const WindowEvent ev = {
				.window			 = window,
				.button			 = static_cast<InputCode>(key),
				.action			 = isRepeated ? InputAction::Repeated : InputAction::Pressed,
				.value			 = Vector2i(scanCode, 0),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_KEYUP: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const WORD keyFlags	  = HIWORD(lParam);
			const WORD scanCode	  = LOBYTE(keyFlags);
			const int  extended	  = (lParam & 0x01000000) != 0;
			const bool isRepeated = (HIWORD(lParam) & KF_REPEAT) != 0;
			uint32_t   key		  = static_cast<uint32_t>(wParam);

			if (wParam == VK_SHIFT)
				key = extended ? VK_LSHIFT : VK_RSHIFT;
			else if (wParam == VK_CONTROL)
				key = extended ? VK_LCONTROL : VK_RCONTROL;

			const WindowEvent ev = {
				.window			 = window,
				.button			 = static_cast<InputCode>(key),
				.action			 = InputAction::Released,
				.value			 = Vector2i(scanCode, 0),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}

		case WM_MOUSEMOVE: {

			const int32_t xPos = GET_X_LPARAM(lParam);
			const int32_t yPos = GET_Y_LPARAM(lParam);

			static Vector2i previousPosition = Vector2i::Zero;
			window->m_mousePosition			 = Vector2i(xPos, yPos);

			const Vector2i delta = window->m_mousePosition - previousPosition;
			previousPosition	 = window->m_mousePosition;

			if (window->m_highFrequencyInputMode)
				return 0;

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseDelta,
				.value			 = delta,
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_MOUSEWHEEL: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int16_t	  delta = GET_WHEEL_DELTA_WPARAM(wParam) / (int16_t)(WHEEL_DELTA);
			const WindowEvent mwe	= {
				  .window		   = window,
				  .type			   = WindowEventType::MouseWheel,
				  .value		   = Vector2i(0, delta),
				  .isHighFrequency = false,
			  };

			window->AddEvent(mwe);

			return 0;
		}
		case WM_LBUTTONDOWN: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse0,
				.action			 = InputAction::Pressed,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_LBUTTONDBLCLK: {

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse0,
				.action			 = InputAction::Repeated,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};
			window->AddEvent(ev);

			return 0;
		}
		case WM_RBUTTONDOWN: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse1,
				.action			 = InputAction::Pressed,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_RBUTTONDBLCLK: {

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse1,
				.action			 = InputAction::Repeated,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_MBUTTONDOWN: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse2,
				.action			 = InputAction::Pressed,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);
			return 0;
		}
		case WM_LBUTTONUP: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse0,
				.action			 = InputAction::Released,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_RBUTTONUP: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse1,
				.action			 = InputAction::Released,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		case WM_MBUTTONUP: {

			if (window->m_highFrequencyInputMode)
				return 0;

			const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
			const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

			const WindowEvent ev = {
				.window			 = window,
				.type			 = WindowEventType::MouseButton,
				.button			 = InputCode::Mouse2,
				.action			 = InputAction::Released,
				.value			 = Vector2i(x, y),
				.isHighFrequency = false,
			};

			window->AddEvent(ev);

			return 0;
		}
		default:
			break;
		}
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}

	Window* Window::Create(uint32_t id, const Vector2i& pos, const Vector2ui& size, const char* title, WindowStyle style)
	{

		HINSTANCE hinst = GetModuleHandle(0);

		WNDCLASSEX wcx;
		BOOL	   exists = GetClassInfoEx(hinst, title, &wcx);

		if (!exists)
		{
			WNDCLASS wc		 = {};
			wc.lpfnWndProc	 = WndProc;
			wc.hInstance	 = hinst;
			wc.lpszClassName = title;
			wc.hCursor		 = NULL;
			wc.style		 = CS_DBLCLKS;

			if (!RegisterClassA(&wc))
			{
				SFG_ERR("Window -> Failed registering window class!");
				return nullptr;
			}
		}

		const DWORD stylew	= GetStyle(style);
		const DWORD exStyle = WS_EX_APPWINDOW;

		Vector2ui trueSize = size;

		if (style == WindowStyle::ApplicationWindow)
		{
			RECT windowRect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
			AdjustWindowRect(&windowRect, stylew, FALSE);
			const int adjustedWidth	 = windowRect.right - windowRect.left;
			const int adjustedHeight = windowRect.bottom - windowRect.top;
			trueSize.x				 = static_cast<uint32_t>(adjustedWidth);
			trueSize.y				 = static_cast<uint32_t>(adjustedHeight);
		}

		HWND hwnd = CreateWindowExA(exStyle, title, title, stylew, pos.x, pos.y, trueSize.x, trueSize.y, NULL, NULL, hinst, NULL);

		if (hwnd == nullptr)
			return nullptr;

		Window* window		   = new Window();
		window->m_id		   = id;
		window->m_osHandle	   = static_cast<void*>(hinst);
		window->m_windowHandle = static_cast<void*>(hwnd);
		window->m_position	   = pos;
		window->m_size		   = size;
		window->m_trueSize	   = trueSize;
		window->m_monitorInfo  = FetchMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));
		window->m_style		   = style;

		// Store user data
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

		// Listen to raw input WM_INPUT
		RAWINPUTDEVICE Rid[2];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage	   = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags	   = RIDEV_INPUTSINK;
		Rid[0].hwndTarget  = hwnd;

		Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[1].usUsage	   = HID_USAGE_GENERIC_KEYBOARD;
		Rid[1].dwFlags	   = RIDEV_INPUTSINK;
		Rid[1].hwndTarget  = hwnd;
		RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]));

		window->BringToFront();
		ShowWindow(hwnd, SW_SHOW);
		return window;
	}

	void Window::Destroy(Window* window)
	{
		DestroyWindow(static_cast<HWND>(window->m_windowHandle));
		delete window;
	}

	void Window::SetPosition(const Vector2i& position)
	{
		HWND hwnd = static_cast<HWND>(m_windowHandle);
		SetWindowPos(hwnd, NULL, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	}

	void Window::SetSize(const Vector2ui& size)
	{
		HWND hwnd = static_cast<HWND>(m_windowHandle);

		m_size		= size;
		m_trueSize	= m_size;
		m_sizeDirty = true;

		if (m_style == WindowStyle::ApplicationWindow)
		{
			RECT windowRect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
			AdjustWindowRect(&windowRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);

			const int adjustedWidth	 = windowRect.right - windowRect.left;
			const int adjustedHeight = windowRect.bottom - windowRect.top;
			m_trueSize.x			 = static_cast<uint32_t>(adjustedWidth);
			m_trueSize.y			 = static_cast<uint32_t>(adjustedHeight);
		}

		SetWindowPos(hwnd, NULL, 0, 0, m_trueSize.x, m_trueSize.y, SWP_NOMOVE | SWP_NOZORDER);
	}

	void Window::SetStyle(WindowStyle style)
	{
		HWND hwnd = static_cast<HWND>(m_windowHandle);
		SetWindowLongPtr(hwnd, GWL_STYLE, GetStyle(style));
		SetWindowPos(hwnd, HWND_TOPMOST, m_position.x, m_position.y, m_trueSize.x, m_trueSize.y, SWP_SHOWWINDOW);
	}

	void Window::BringToFront()
	{
		HWND hwnd = static_cast<HWND>(m_windowHandle);
		OpenIcon(hwnd);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

} // namespace SFG

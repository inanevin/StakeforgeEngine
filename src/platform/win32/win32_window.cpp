// Copyright (c) 2025 Inan Evin

#include "platform/window.hpp"
#include "io/log.hpp"
#include "input/input_common.hpp"
#include "input/input_mappings.hpp"
#include "math/math.hpp"
#include <windowsx.h>
#include <hidusage.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Shcore.lib")

namespace Game
{
	window::map window::s_key_down_map = {};

	namespace
	{
		auto composition_enabled() -> bool
		{
			BOOL composition_enabled = FALSE;
			bool success			 = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
			return composition_enabled && success;
		}

		uint32 get_style(uint8 flags)
		{
			if (flags & window_flags::wf_style_windowed)
				return static_cast<uint32>(WS_OVERLAPPEDWINDOW);
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

		monitor_info fetch_monitor_info(HMONITOR monitor)
		{
			monitor_info info;

			MONITORINFOEX monitor_info;
			monitor_info.cbSize = sizeof(monitor_info);
			GetMonitorInfo(monitor, &monitor_info);

			UINT	dpiX, dpiY;
			HRESULT temp2	= GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
			info.size		= {static_cast<uint32>(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left), static_cast<uint32>(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top)};
			info.work_size	= {static_cast<uint32>(monitor_info.rcWork.right - monitor_info.rcWork.left), static_cast<uint32>(monitor_info.rcWork.bottom - monitor_info.rcWork.top)};
			info.position	= {static_cast<int32>(monitor_info.rcWork.left), static_cast<int32>(monitor_info.rcWork.top)};
			info.is_primary = (monitor_info.dwFlags & MONITORINFOF_PRIMARY) != 0;
			info.dpi		= dpiX;
			info.dpi_scale	= static_cast<float>(dpiX) / 96.0f;
			return info;
		}
	} // namespace

	LRESULT __stdcall window::wnd_proc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
	{
		window* wnd = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
		case WM_DPICHANGED:
		case WM_DISPLAYCHANGE: {
			wnd->_monitor_info = fetch_monitor_info(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));
			break;
		}
		case WM_CLOSE: {
			wnd->_flags.set(window_flags::wf_close_requested);
			return 0;
		}
		case WM_KILLFOCUS: {
			wnd->_flags.remove(wf_has_focus);
			return 0;
		}
		case WM_SETFOCUS: {
			wnd->_flags.set(wf_has_focus);
			return 0;
		}
		case WM_MOVE: {
			const int32 x  = static_cast<int32>((short)LOWORD(lParam));
			const int32 y  = static_cast<int32>((short)HIWORD(lParam));
			wnd->_position = vector2i(x, y);
			return 0;
		}
		case WM_SIZE: {

			UINT width	= LOWORD(lParam);
			UINT height = HIWORD(lParam);

			RECT rect;
			GetWindowRect(hwnd, &rect);

			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			wnd->_size = vector2ui(width, height);
			wnd->_flags.set(wf_size_dirty);

			if (wnd->_flags.is_set(window_flags::wf_style_windowed))
			{
				wnd->_true_size.x = rect.right - rect.left;
				wnd->_true_size.y = rect.bottom - rect.top;
			}
			return 0;
		}
		case WM_INPUT: {

			if (!wnd->_flags.is_set(wf_high_freq))
				return 0;

			UINT		dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				// Handle keyboard input
				const USHORT key		= raw->data.keyboard.VKey;
				USHORT		 scanCode	= raw->data.keyboard.MakeCode;
				const bool	 is_release = raw->data.keyboard.Flags & RI_KEY_BREAK;

				uint8 is_repeat = 0;
				if (!is_release)
				{
					is_repeat			= s_key_down_map[key];
					s_key_down_map[key] = 1;
				}
				else
					s_key_down_map[key] = 0;

				const window_event ev = {
					.value	  = vector2i(static_cast<int32>(scanCode), 0),
					.button	  = static_cast<input_code>(key),
					.type	  = window_event_type::key,
					.sub_type = is_release ? window_event_sub_type::release : (is_repeat ? window_event_sub_type::repeat : window_event_sub_type::press),
					.flags	  = wef_high_freq,
				};
				wnd->add_event(ev);
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				USHORT mouse_flags = raw->data.mouse.usButtonFlags;
				POINT  cursorPos;
				GetCursorPos(&cursorPos);

				wnd->_mouse_position_abs = vector2i(static_cast<int32>(cursorPos.x), static_cast<int32>(cursorPos.y));

				const vector2i relative = wnd->_mouse_position_abs - wnd->_position;
				wnd->_mouse_position	= vector2i::clamp(relative, vector2i(), vector2i(wnd->_size));

				window_event ev = {

					.value = relative,
					.type  = window_event_type::mouse,
					.flags = wef_high_freq,
				};

				bool ev_exists = false;

				if (mouse_flags & RI_MOUSE_LEFT_BUTTON_DOWN)
				{
					ev.button	= input_code::Mouse0;
					ev.sub_type = window_event_sub_type::press;
					ev_exists	= true;
				}
				if (mouse_flags & RI_MOUSE_LEFT_BUTTON_UP)
				{
					ev.button	= input_code::Mouse0;
					ev.sub_type = window_event_sub_type::release;
					ev_exists	= true;
				}
				if (mouse_flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
				{
					ev.button	= input_code::Mouse1;
					ev.sub_type = window_event_sub_type::press;
					ev_exists	= true;
				}
				if (mouse_flags & RI_MOUSE_RIGHT_BUTTON_UP)
				{
					ev.button	= input_code::Mouse1;
					ev.sub_type = window_event_sub_type::release;
					ev_exists	= true;
				}
				if (mouse_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				{
					ev.button	= input_code::Mouse2;
					ev.sub_type = window_event_sub_type::press;
					ev_exists	= true;
				}
				if (mouse_flags & RI_MOUSE_MIDDLE_BUTTON_UP)
				{
					ev.button	= input_code::Mouse2;
					ev.sub_type = window_event_sub_type::release;
					ev_exists	= true;
				}

				if (ev_exists)
					wnd->add_event(ev);
				else if (mouse_flags & RI_MOUSE_WHEEL)
				{
					const uint16 wheelDelta = (uint16)raw->data.mouse.usButtonData;
					const short	 wheel		= (short)raw->data.mouse.usButtonData / (short)WHEEL_DELTA;

					const window_event mwe = {
						.value = vector2i(0, wheel),
						.type  = window_event_type::wheel,
						.flags = wef_high_freq,
					};
					wnd->add_event(mwe);
				}
				else
				{
					const int32 xPosRelative = raw->data.mouse.lLastX;
					const int32 yPosRelative = raw->data.mouse.lLastY;

					const window_event mdEvent = {
						.value = vector2i(xPosRelative, yPosRelative),
						.type  = window_event_type::delta,
						.flags = wef_high_freq,
					};

					wnd->add_event(mdEvent);
				}
			}
			return 0;
		}
		case WM_KEYDOWN: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const WORD key_flags = HIWORD(lParam);
			const WORD scanCode	 = LOBYTE(key_flags);
			const int  extended	 = (lParam & 0x01000000) != 0;
			const bool is_repeat = (lParam & 1 << 30);
			uint32	   key		 = static_cast<uint32>(wParam);

			if (wParam == VK_SHIFT)
				key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
			else if (wParam == VK_CONTROL)
				key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

			const window_event ev = {

				.value	  = vector2i(scanCode, 0),
				.button	  = static_cast<input_code>(key),
				.type	  = window_event_type::key,
				.sub_type = is_repeat ? window_event_sub_type::repeat : window_event_sub_type::press,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_KEYUP: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const WORD key_flags = HIWORD(lParam);
			const WORD scanCode	 = LOBYTE(key_flags);
			const int  extended	 = (lParam & 0x01000000) != 0;
			uint32	   key		 = static_cast<uint32>(wParam);

			if (wParam == VK_SHIFT)
				key = extended ? VK_LSHIFT : VK_RSHIFT;
			else if (wParam == VK_CONTROL)
				key = extended ? VK_LCONTROL : VK_RCONTROL;

			const window_event ev = {

				.value = vector2i(scanCode, 0), .button = static_cast<input_code>(key), .type = window_event_type::key, .sub_type = window_event_sub_type::release};

			wnd->add_event(ev);

			return 0;
		}

		case WM_MOUSEMOVE: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 xPos = GET_X_LPARAM(lParam);
			const int32 yPos = GET_Y_LPARAM(lParam);

			static vector2i previousPosition = vector2i();
			wnd->_mouse_position			 = vector2i(xPos, yPos);
			wnd->_mouse_position_abs		 = wnd->get_position() + wnd->_mouse_position;

			const vector2i delta = wnd->_mouse_position - previousPosition;
			previousPosition	 = wnd->_mouse_position;

			const window_event ev = {
				.value = delta,
				.type  = window_event_type::delta,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_MOUSEWHEEL: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;
			const int16		   delta = GET_WHEEL_DELTA_WPARAM(wParam) / (int16)(WHEEL_DELTA);
			const window_event mwe	 = {
				  .value = vector2i(0, delta),
				  .type	 = window_event_type::wheel,
			  };

			wnd->add_event(mwe);

			return 0;
		}
		case WM_LBUTTONDOWN: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse0,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::press,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_LBUTTONDBLCLK: {

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse0,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::repeat,
			};
			wnd->add_event(ev);

			return 0;
		}
		case WM_RBUTTONDOWN: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse1,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::press,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_RBUTTONDBLCLK: {

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse1,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::repeat,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_MBUTTONDOWN: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse2,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::press,
			};

			wnd->add_event(ev);
			return 0;
		}
		case WM_LBUTTONUP: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse0,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::release,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_RBUTTONUP: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse1,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::release,
			};

			wnd->add_event(ev);

			return 0;
		}
		case WM_MBUTTONUP: {

			if (wnd->_flags.is_set(wf_high_freq))
				return 0;

			const int32 x = static_cast<int32>(GET_X_LPARAM(lParam));
			const int32 y = static_cast<int32>(GET_Y_LPARAM(lParam));

			const window_event ev = {

				.value	  = vector2i(x, y),
				.button	  = input_code::Mouse2,
				.type	  = window_event_type::mouse,
				.sub_type = window_event_sub_type::release,
			};

			wnd->add_event(ev);

			return 0;
		}
		default:
			break;
		}
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}

	bool window::create(const char* title, uint8 flags, const vector2i& pos, const vector2ui& size)
	{
		HINSTANCE  hinst = GetModuleHandle(0);
		WNDCLASSEX wcx;
		BOOL	   exists = GetClassInfoEx(hinst, title, &wcx);

		if (!exists)
		{
			WNDCLASS wc		 = {};
			wc.lpfnWndProc	 = wnd_proc;
			wc.hInstance	 = hinst;
			wc.lpszClassName = title;
			wc.hCursor		 = NULL;
			wc.style		 = CS_DBLCLKS;

			if (!RegisterClassA(&wc))
			{
				GAME_ERR("Failed registering window class!");
				return false;
			}
		}

		const DWORD stylew	= get_style(flags);
		const DWORD exStyle = WS_EX_APPWINDOW;

		_position = pos;
		_size	  = size;
		_flags	  = flags;

		if (flags & window_flags::wf_style_windowed)
		{
			RECT windowRect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
			AdjustWindowRect(&windowRect, stylew, FALSE);
			const int adjustedWidth	 = windowRect.right - windowRect.left;
			const int adjustedHeight = windowRect.bottom - windowRect.top;
			_true_size.x			 = static_cast<uint32>(adjustedWidth);
			_true_size.y			 = static_cast<uint32>(adjustedHeight);
		}

		HWND hwnd = CreateWindowExA(exStyle, title, title, stylew, _position.x, _position.y, _true_size.x, _true_size.y, NULL, NULL, hinst, NULL);
		if (!hwnd)
			return false;

		_platform_handle = static_cast<void*>(hinst);
		_window_handle	 = static_cast<void*>(hwnd);
		_monitor_info	 = fetch_monitor_info(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

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

		bring_to_front();
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		return true;
	}

	void window::destroy()
	{
		if (_window_handle)
			DestroyWindow(static_cast<HWND>(_window_handle));
	}

	void window::set_position(const vector2i& pos)
	{
		HWND hwnd = static_cast<HWND>(_window_handle);
		SetWindowPos(hwnd, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	}

	void window::set_size(const vector2i& size)
	{
		HWND hwnd = static_cast<HWND>(_window_handle);

		_size = size;
		_flags.set(wf_size_dirty);

		if (_flags.is_set(window_flags::wf_style_windowed))
		{
			RECT windowRect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
			AdjustWindowRect(&windowRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);

			const int adjusted_width  = windowRect.right - windowRect.left;
			const int adjusted_height = windowRect.bottom - windowRect.top;
			_true_size.x			  = static_cast<uint32>(adjusted_width);
			_true_size.y			  = static_cast<uint32>(adjusted_height);
		}

		SetWindowPos(hwnd, NULL, 0, 0, _true_size.x, _true_size.y, SWP_NOMOVE | SWP_NOZORDER);
	}

	void window::set_style(window_flags flags)
	{
		HWND hwnd = static_cast<HWND>(_window_handle);
		SetWindowLongPtr(hwnd, GWL_STYLE, get_style(flags));
		SetWindowPos(hwnd, HWND_TOPMOST, _position.x, _position.y, _true_size.x, _true_size.y, SWP_SHOWWINDOW);
	}

	void window::bring_to_front()
	{
		HWND hwnd = static_cast<HWND>(_window_handle);
		OpenIcon(hwnd);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	void window::add_event(const window_event& ev)
	{
		_events[_event_count] = ev;
		_event_count		  = (_event_count + 1) % MAX_EVENTS;
	}
}
// Copyright (c) 2025 Inan Evin

#include "platform/process.hpp"
#include "platform/window_common.hpp"
#include "io/assert.hpp"

#include "io/log.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>

namespace
{
	int enumerate_monitors(HMONITOR monitor, HDC, LPRECT, LPARAM l_param)
	{
		SFG::vector<SFG::monitor_info>* infos = reinterpret_cast<SFG::vector<SFG::monitor_info>*>(l_param);
		infos->push_back({});
		SFG::monitor_info& info = infos->back();

		MONITORINFOEX monitor_info;
		monitor_info.cbSize = sizeof(monitor_info);
		GetMonitorInfo(monitor, &monitor_info);

		UINT	dpiX, dpiY;
		HRESULT temp2	= GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
		info.size		= {static_cast<uint16>(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left), static_cast<uint16>(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top)};
		info.work_size	= {static_cast<uint16>(monitor_info.rcWork.right - monitor_info.rcWork.left), static_cast<uint16>(monitor_info.rcWork.bottom - monitor_info.rcWork.top)};
		info.position	= {static_cast<int16>(monitor_info.rcWork.left), static_cast<int16>(monitor_info.rcWork.top)};
		info.is_primary = (monitor_info.dwFlags & MONITORINFOF_PRIMARY) != 0;
		info.dpi		= dpiX;
		info.dpi_scale	= static_cast<float>(dpiX) / 96.0f;
		return 1;
	}
}

namespace SFG
{
	string process::select_folder(const char* title)
	{
		string result;

		IFileDialog* dialog = nullptr;
		HRESULT		 hr		= CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));

		if (SUCCEEDED(hr))
		{
			DWORD options;
			if (SUCCEEDED(dialog->GetOptions(&options)))
			{
				dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
			}

			hr = dialog->Show(nullptr);
			if (SUCCEEDED(hr))
			{
				IShellItem* item = nullptr;
				if (SUCCEEDED(dialog->GetResult(&item)))
				{
					PWSTR path = nullptr;
					if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
					{
						char buffer[MAX_PATH];
						WideCharToMultiByte(CP_UTF8, 0, path, -1, buffer, MAX_PATH, nullptr, nullptr);
						result = buffer;
						CoTaskMemFree(path);
					}
					item->Release();
				}
			}
			dialog->Release();
		}

		return result;
	}

	void process::pump_os_messages()
	{
		MSG msg	   = {0};
		msg.wParam = 0;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void process::open_url(const char* url)
	{
		ShellExecute(0, "open", url, NULL, NULL, SW_SHOWNORMAL);
	}

	void process::message_box(const char* msg)
	{
		MessageBox(nullptr, msg, "Huh?", MB_OK | MB_ICONERROR);
	}
	void process::get_all_monitors(vector<monitor_info>& out)
	{
		EnumDisplayMonitors(NULL, NULL, enumerate_monitors, (LPARAM)&out);
	}

	char process::get_character_from_key(uint32 key)
	{
		BYTE keyboard_states[256];
		if (!GetKeyboardState(keyboard_states))
			return 0;

		SHORT shiftState = GetAsyncKeyState(VK_SHIFT);
		if ((shiftState & 0x8000) != 0) // High bit set = key is down
		{
			keyboard_states[VK_SHIFT] |= 0x80;
		}

		UINT  scan_code = MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
		WCHAR buffer[4] = {};
		int	  result	= ToUnicodeEx(key, scan_code, keyboard_states, buffer, ARRAYSIZE(buffer), 0, GetKeyboardLayout(0));

		if (result == 1)
			return static_cast<char>(buffer[0]); // Return ASCII character
		else
			return '\0'; // Non-printable or failed
	}

	uint16 SFG::process::get_character_mask_from_key(uint32 keycode, char ch)
	{
		uint16 mask = 0;

		if (ch == L' ')
			mask |= whitespace;
		else
		{
			if (IsCharAlphaNumericA(ch))
			{
				if (keycode >= '0' && keycode <= '9')
					mask |= number;
				else if ((keycode >= '0' && keycode <= '9') || (keycode >= VK_NUMPAD0 && keycode <= VK_NUMPAD9))
				{
					mask |= number;
				}
				else
					mask |= letter;
			}
			else if (iswctype(ch, _PUNCT))
			{
				mask |= symbol;

				if (ch == '-' || ch == '+' || ch == '*' || ch == '/')
					mask |= op;

				if (ch == L'-' || ch == L'+')
					mask |= sign;
			}
			else
				mask |= control;
		}

		if (ch == L'.' || ch == L',')
			mask |= separator;

		if (mask & (letter | number | whitespace | separator | symbol))
			mask |= printable;

		return mask;
	}

	/*
	void process::send_pipe_data(void* data, size_t data_size)
	{
		HANDLE pipe = static_cast<HANDLE>(s_pipe_handle);
		if (pipe == INVALID_HANDLE_VALUE || pipe == nullptr)
			return;

		SFG_ASSERT(data_size < PIPE_MAX_MSG_SIZE);

		DWORD bytesWritten;
		if (!WriteFile(pipe, data, static_cast<DWORD>(data_size), &bytesWritten, NULL))
		{
			DWORD dwError = GetLastError();
			// SFG_ERR("send_pipe_data() -> Failed to write to pipe: {0}", dwError);
		}
	}*/

} // namespace SFG

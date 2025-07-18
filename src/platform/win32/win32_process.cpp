// Copyright (c) 2025 Inan Evin

#include "platform/process.hpp"
#include "io/log.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

namespace Game
{
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
} // namespace SFG

// Copyright (c) 2025 Inan Evin

#include "log.hpp"
#include "data/string.hpp"

#ifdef GAME_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Game
{
	void log::log_impl(log::level level, const char* msg)
	{
		LOCK_GUARD(_mtx);

		const string msgStr = "[" + string(get_level(level)) + "] " + msg + "\n";

#ifdef GAME_PLATFORM_WINDOWS
		HANDLE hConsole;
		int	   color = 15;

		if (level == level::trace)
			color = 3;
		else if (level == level::info)
			color = 15;
		else if ((level == level::warning))
			color = 6;
		else if (level == level::error)
			color = 4;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msgStr.c_str(), static_cast<DWORD>(strlen(msgStr.c_str())), NULL, NULL);
#else
		std::cout << msgStr.c_str();
#endif
	}

	const char* log::get_level(level level)
	{
		switch (level)
		{
		case level::error:
			return "Error";
		case level::info:
			return "Info";
		case level::trace:
			return "Trace";
		case level::warning:
			return "Warn";
		default:
			return "";
		}
	}

}

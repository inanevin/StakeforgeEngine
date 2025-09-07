// Copyright (c) 2025 Inan Evin

#include "log.hpp"
#include "data/vector_util.hpp"
#include "data/string.hpp"

#ifdef SFG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace SFG
{
	void log::log_impl(log_level level, const char* msg)
	{
		LOCK_GUARD(_mtx);

		const string msgStr = "[" + string(get_level(level)) + "] " + msg + "\n";

#ifdef SFG_PLATFORM_WINDOWS
		HANDLE hConsole;
		int	   color = 15;

		if (level == log_level::trace)
			color = 3;
		else if (level == log_level::info)
			color = 15;
		else if ((level == log_level::warning))
			color = 6;
		else if (level == log_level::error)
			color = 4;
		else if (level == log_level::progress)
			color = 8;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msgStr.c_str(), static_cast<DWORD>(strlen(msgStr.c_str())), NULL, NULL);
#else
		std::cout << msgStr.c_str();
#endif

		/*
				ostream		stream;
				const uint8 dt		  = (uint8)pipe_data_type::log;
				const uint8 log_level = (uint8)level;
				stream << dt;
				stream << level;
				stream.write_raw((uint8*)msg, strlen(msg));
				process::send_pipe_data(stream.get_raw(), stream.get_size());
				stream.destroy();
		*/

		for (const listener& l : _listeners)
			l.f(level, msg);
	}

	void log::add_listener(unsigned int id, callback_function f)
	{
		_listeners.push_back({.id = id, .f = f});
	}

	void log::remove_listener(unsigned int id)
	{
		std::erase_if(_listeners, [id](const listener& l) -> bool { return l.id == id; });
	}

	const char* log::get_level(log_level level)
	{
		switch (level)
		{
		case log_level::error:
			return "Error";
		case log_level::info:
			return "Info";
		case log_level::trace:
			return "Trace";
		case log_level::warning:
			return "Warn";
		case log_level::progress:
			return "Progress";
		default:
			return "";
		}
	}

}

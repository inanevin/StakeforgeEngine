// Copyright (c) 2025 Inan Evin

#include "debug_console.hpp"

namespace Game
{
	debug_console* debug_console::s_instance = nullptr;

	void debug_console::parse_console_command(const string& cmd)
	{
		const size_t ws = cmd.find_first_of(" ");

		if (ws == string::npos)
		{
			const string_id sid = TO_SID(cmd);
			auto			it	= _console_entries.find(sid);
			if (it == _console_entries.end())
			{
				GAME_ERR("debug_console::parse_console_command() -> command is not a function, and if it's a variable no argument is provided!");
				return;
			}

			it->second->execute("");
			return;
		}

		const string	func = cmd.substr(0, ws);
		const string_id sid	 = TO_SID(func);
		auto			it	 = _console_entries.find(sid);
		if (it == _console_entries.end())
		{
			GAME_ERR("debug_console::parse_console_command() -> command can not be found!");
			return;
		}

		const string args = cmd.substr(ws + 1, cmd.length() - ws);
		it->second->execute(args);
	}

	void debug_console::init()
	{
		s_instance = new debug_console();
		s_instance->_console_entries.reserve(256);
	}

	void debug_console::uninit()
	{
		for (auto [sid, ptr] : s_instance->_console_entries)
			delete ptr;
		s_instance->_console_entries.clear();
		delete s_instance;
		s_instance = nullptr;
	}
}
// Copyright (c) 2025 Inan Evin

#include "debug_console.hpp"

namespace Game
{
	debug_console* debug_console::s_instance = nullptr;

	void debug_console::parse_console_command(const char* cmd)
	{
		string str = cmd;
		string_util::remove_whitespace(str);

		const size_t eq = str.find_first_of("=");

		if (eq != string::npos)
		{
			const string	command = str.substr(0, eq);
			const string_id sid		= TO_SID(command);
			auto			it		= _console_entries.find(sid);
			if (it == _console_entries.end())
			{
				SFG_ERR("debug_console::parse_console_command() -> command seems to be a variable, but it can't be found!");
				return;
			}
			const string args = str.substr(eq + 1, str.size() - eq - 1);
			it->second->execute(args);
			return;
		}

		const size_t bracket0 = str.find_first_of("(");
		const size_t bracket1 = str.find_first_of(")");

		if (bracket0 == string::npos || bracket1 == string::npos)
		{
			SFG_ERR("debug_console::parse_console_command() -> command is not a variable, and can't be recognize as a function because of missing bracket(s)");
			return;
		}

		const string command = str.substr(0, bracket0);

		const string_id sid = TO_SID(command);
		auto			it	= _console_entries.find(sid);
		if (it == _console_entries.end())
		{
			SFG_ERR("debug_console::parse_console_command() -> command seems to be a function, but can't be found!");
			return;
		}

		const string args = str.substr(bracket0 + 1, bracket1 - bracket0 - 1);
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
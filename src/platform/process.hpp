// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
#include "common/size_definitions.hpp"

namespace Game
{
	struct monitor_info;

	class process
	{
	public:
		static void pump_os_messages();
		static void open_url(const char* url);
		static void message_box(const char* msg);
		static void get_all_monitors(vector<monitor_info>& out);
		static char get_character_from_key(uint32 key);
	};

} // namespace SFG

// Copyright (c) 2025 Inan Evin

#pragma once

namespace Game
{
	class process
	{
	public:
		static void pump_os_messages();
		static void open_url(const char* url);
		static void message_box(const char* msg);
	};

} // namespace SFG

// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
#include "common/size_definitions.hpp"

namespace Game
{
	struct monitor_info;

	enum character_mask
	{
		letter	   = 1 << 0,
		number	   = 1 << 1,
		separator  = 1 << 2,
		symbol	   = 1 << 4,
		whitespace = 1 << 5,
		control	   = 1 << 6,
		printable  = 1 << 7,
		op		   = 1 << 8,
		sign	   = 1 << 9,
	};

	class process
	{
	public:
		static void	  pump_os_messages();
		static void	  open_url(const char* url);
		static void	  message_box(const char* msg);
		static void	  get_all_monitors(vector<monitor_info>& out);
		static char	  get_character_from_key(uint32 key);
		static uint16 get_character_mask_from_key(uint32 key, char ch);
	};

} // namespace SFG

// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
#include "data/string.hpp"
#include "common/size_definitions.hpp"

namespace SFG
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
		static string select_folder(const char* title);

		/*
		static void send_pipe_data(void* data, size_t data_size);

		static inline void set_pipe_handle(void* hnd)
		{
			s_pipe_handle = hnd;
		}

		static inline void* get_pipe_handle()
		{
			return s_pipe_handle;
		}
		*/

	private:
		/*
				static void* s_pipe_handle;
		*/
	};

} // namespace SFG

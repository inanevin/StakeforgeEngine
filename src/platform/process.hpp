// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
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

#ifdef SFG_TOOLMODE
#define PIPE_MAX_MSG_SIZE 4096

	enum class pipe_data_type : uint8
	{
		log,
	};
#endif

	class process
	{
	public:
		static void	  pump_os_messages();
		static void	  open_url(const char* url);
		static void	  message_box(const char* msg);
		static void	  get_all_monitors(vector<monitor_info>& out);
		static char	  get_character_from_key(uint32 key);
		static uint16 get_character_mask_from_key(uint32 key, char ch);

#ifdef SFG_TOOLMODE

		/* pipe_data_type + data itself */
		static void send_pipe_data(void* data, size_t data_size);

		static inline void set_pipe_handle(void* hnd)
		{
			s_pipe_handle = hnd;
		}

		static inline void* get_pipe_handle()
		{
			return s_pipe_handle;
		}

#endif

	private:
#ifdef SFG_TOOLMODE

		static void* s_pipe_handle;

#endif
	};

} // namespace SFG

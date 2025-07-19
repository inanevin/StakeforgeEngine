// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "command_buffer.hpp"

namespace Game
{
	struct semaphore_data
	{
		uint16 semaphore = 0;
		uint64 value	 = 0;
	};

	struct command_group
	{
		command_buffer* buffers		  = nullptr;
		semaphore_data* waits		  = nullptr;
		semaphore_data* submits		  = nullptr;
		uint8			waits_count	  = 0;
		uint8			submits_count = 0;
		uint8			buffers_count = 0;
	};
}
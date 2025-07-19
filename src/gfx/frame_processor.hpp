// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "frame_common.hpp"

namespace Game
{

#define MAX_SEMAPHORE_DATA 64
#define MAX_CMD_GROUPS	   64

	class frame_processor
	{
	public:
		command_group command_groups[MAX_CMD_GROUPS];
		uint16		  command_group_count = 0;

		command_group& add_command_group()
		{
			return command_groups[command_group_count++];
		}

		void process()
		{
			for (uint16 i = 0; i < command_group_count; i++)
			{
				execute_command_group(command_groups[i]);
			}
		}

		void execute_command_group(command_group& group)
		{
			// record commands.
			// submit commands.
			// submission waits for wait_semaphores
			// submission signals signal_semaphores

			for (uint8 i = 0; i < group.buffers_count; i++)
			{
				execute_command_buffer(group.buffers[i]);
				submit_to_queue(group.buffers[i], 0);
			}
		}

		void execute_command_buffer(command_buffer& buffer)
		{
		}

		void submit_to_queue(command_buffer& buffer, uint8 queue)
		{
		}
	};

}
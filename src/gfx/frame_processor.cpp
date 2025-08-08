// Copyright (c) 2025 Inan Evin

#include "frame_processor.hpp"
#include "memory/memory.hpp"
#include "math/math_common.hpp"
#include "gfx/backend/backend.hpp"
#include <algorithm>
#include <execution>
#include <ranges>

namespace Game
{
	void frame_processor::init(size_t processor_memory_size, size_t aux_size)
	{
		_processor_memory_size = ALIGN_UP(processor_memory_size, 8);
		_processor_aux_size	   = ALIGN_UP(aux_size, 8);
		_raw				   = reinterpret_cast<uint8*>(SFG_ALIGNED_MALLOC(8, processor_memory_size + aux_size));
		_head_processor		   = 0;
		_head_aux			   = _processor_memory_size;
	}

	void frame_processor::uninit()
	{
		SFG_ALIGNED_FREE(_raw);
	}

	void frame_processor::reset()
	{
		_counter		= 0;
		_head_processor = 0;
		_head_aux		= _processor_memory_size;
	}

	void frame_processor::process()
	{
		// uint8*		 ptr	 = _raw;
		gfx_backend* backend = gfx_backend::get();
		size_t		 offset	 = 0;

		for (uint8 i = 0; i < _counter; i++)
		{
			uint8*			  ptr	 = _raw + offset;
			processor_header* header = reinterpret_cast<processor_header*>(ptr);

			if (header->flags.is_set(processor_event_flags::pef_is_cpu_wait))
			{
				processor_event_cpu_wait* event = reinterpret_cast<processor_event_cpu_wait*>(ptr);
				offset += sizeof(processor_event_cpu_wait);
				// ptr += sizeof(processor_event_cpu_wait);

				for (uint16 j = 0; j < event->wait_count; j++)
					backend->wait_semaphore(event->wait_semaphores[i], event->wait_values[i]);
			}
			else if (header->flags.is_set(processor_event_flags::pef_is_present))
			{
				processor_event_present* event = reinterpret_cast<processor_event_present*>(ptr);
				offset += sizeof(processor_event_present);

				backend->present(event->swapchains, event->swapchain_count);
			}
			else
			{
				processor_event_work_group* event = reinterpret_cast<processor_event_work_group*>(ptr);
				offset += sizeof(processor_event_work_group);

				resource_id* cmd_buffers	  = event->command_buffers;
				record_func* record_callbacks = event->command_record_callbacks;
				const int	 count			  = static_cast<int>(event->command_buffer_count);

				std::for_each_n(std::execution::par, cmd_buffers, count, [backend, cmd_buffers, record_callbacks](resource_id& id) {
					const int n = std::distance(cmd_buffers, &id);
					backend->reset_command_buffer(id);
					record_callbacks[n](id);
					backend->close_command_buffer(id);
				});

				if (event->wait_count != 0)
					backend->queue_wait(event->queue, event->wait_semaphores, static_cast<uint8>(event->wait_count), event->wait_values);

				gfx_backend::get()->submit_commands(event->queue, cmd_buffers, count);

				if (event->signal_count != 0)
					backend->queue_signal(event->queue, event->signal_semaphores, static_cast<uint8>(event->signal_count), event->signal_values);
			}
		}
	}

}
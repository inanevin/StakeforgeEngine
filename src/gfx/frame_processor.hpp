// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/gfx_common.hpp"
#include "io/assert.hpp"
#include "memory/memory.hpp"
#include <functional>

namespace Game
{

#define MAX_SEMAPHORE_DATA 64
#define MAX_CMD_GROUPS	   64

	typedef std::function<void(resource_id buffer)> record_func;

	enum processor_event_flags
	{
		pef_is_work_group = 1 << 0,
		pef_is_cpu_wait	  = 1 << 1,
		pef_is_present	  = 1 << 2,
	};

	struct processor_header
	{
		bitmask<uint32> flags;
	};

	struct alignas(8) processor_event_work_group
	{
		processor_header header					  = {};
		resource_id		 queue					  = 0;
		resource_id*	 command_buffers		  = nullptr;
		record_func*	 command_record_callbacks = nullptr;
		resource_id*	 wait_semaphores		  = nullptr;
		resource_id*	 signal_semaphores		  = nullptr;
		uint64*			 wait_values			  = nullptr;
		uint64*			 signal_values			  = nullptr;
		uint16			 command_buffer_count	  = 0;
		uint16			 wait_count				  = 0;
		uint16			 signal_count			  = 0;
	};

	struct alignas(8) processor_event_cpu_wait
	{
		processor_header header			 = {};
		resource_id*	 wait_semaphores = nullptr;
		uint64*			 wait_values	 = nullptr;
		uint16			 wait_count		 = 0;
	};

	struct alignas(8) processor_event_present
	{
		processor_header header			 = {};
		resource_id*	 swapchains		 = 0;
		uint8			 swapchain_count = 0;
	};

	class frame_processor
	{
	private:
	public:
		void init(size_t processor_memory_size, size_t aux_memory);
		void uninit();
		void reset();
		void process();

		processor_event_work_group* add_work_group()
		{
			return add_event<processor_event_work_group>(processor_event_flags::pef_is_work_group);
		}
		processor_event_cpu_wait* add_wait_event()
		{
			return add_event<processor_event_cpu_wait>(processor_event_flags::pef_is_cpu_wait);
		}
		inline processor_event_present* add_present_event()
		{
			return add_event<processor_event_present>(processor_event_flags::pef_is_present);
		}

		template <typename T> T* allocate_aux(uint32 count)
		{
			const size_t raw_size	   = sizeof(T) * count;
			const size_t required_size = (raw_size + 8 - 1) & ~(8 - 1);
			SFG_ASSERT(_head_aux + required_size < _processor_memory_size + _processor_aux_size);
			uint8* ptr = _raw + _head_aux;
			_head_aux += required_size;
			return reinterpret_cast<T*>(ptr);
		}

		template <typename T, typename... Args> T* emplace_aux(T firstValue, Args&&... remainingValues)
		{
			SFG_ASSERT(_head_aux + sizeof(T) < _processor_memory_size + _processor_aux_size);
			const size_t aligned = ALIGN_UP(_head_aux, alignof(T));
			uint8*		 ptr	 = _raw + aligned;

			SFG_MEMCPY(ptr, &firstValue, sizeof(T));
			_head_aux = aligned + sizeof(T);

			if constexpr (sizeof...(remainingValues) > 0)
			{
				emplace_aux<T>(remainingValues...);
			}

			return reinterpret_cast<T*>(ptr);
		}

	private:
		template <typename T> T* add_event(uint8 flags)
		{
			const size_t required_sz = sizeof(T);
			SFG_ASSERT(_head_processor + required_sz < _processor_memory_size);

			uint8* ptr = _raw + _head_processor;
			_head_processor += required_sz;

			new (ptr) T();

			processor_header* header = reinterpret_cast<processor_header*>(ptr);
			header->flags			 = flags;
			_counter++;
			return reinterpret_cast<T*>(ptr);
		}

	private:
		uint8* _raw			   = nullptr;
		size_t _head_processor = 0;
		size_t _head_aux	   = 0;

		size_t _processor_memory_size = 0;
		size_t _processor_aux_size	  = 0;
		uint16 _counter				  = 0;
	};

}
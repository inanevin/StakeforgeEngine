// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_common.hpp"
#include "data/span.hpp"

namespace Game
{
	struct command_header
	{
		uint8 tid	  = 0;
		uint8 padding = 0;
	};

	class command_buffer
	{
		template <typename T> T* add_command()
		{
			constexpr size_t align = alignof(T);
			constexpr size_t size  = sizeof(T);

			// Align the next command after the header
			const size_t header_size	= sizeof(command_header);
			const size_t header_offset	= _current_offset;
			const size_t command_offset = (header_offset + header_size + (align - 1)) & ~(align - 1);
			const size_t padding_needed = command_offset - _current_offset;

			// Write header
			command_header* header = reinterpret_cast<command_header*>(&_data.data[header_offset]);
			header->tid			   = T::TID;
			header->padding		   = 0;

			_current_offset += padding_needed + size;

			// Return pointer to aligned command
			return reinterpret_cast<T*>(&_data.data[command_offset]);
		}

	private:
		span<uint8> _data			= {};
		size_t		_current_offset = 0;
	};
}
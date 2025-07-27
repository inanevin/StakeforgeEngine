// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "data/bitmask.hpp"

namespace Game
{
	struct resource_desc;

	class buffer
	{
	public:
		void create_staging_hw(const resource_desc& staging, const resource_desc& hw);
		void create_hw(const resource_desc& desc);
		void destroy();
		void buffer_data(size_t padding, const void* data, size_t size);
		bool copy(resource_id cmd_buffer);

		inline resource_id get_hw_staging() const
		{
			return _hw_staging;
		}
		inline resource_id get_hw_gpu() const
		{
			return _hw_gpu;
		}

		inline bool get_is_modified() const
		{
			return _flags.is_set(buffer_flags::buf_modified);
		}

	private:
		enum buffer_flags
		{
			buf_has_staging = 1 << 0,
			buf_modified	= 1 << 2,
		};

	private:
		uint8*		   _mapped	   = nullptr;
		resource_id	   _hw_staging = 0;
		resource_id	   _hw_gpu	   = 0;
		bitmask<uint8> _flags	   = 0;
	};
}

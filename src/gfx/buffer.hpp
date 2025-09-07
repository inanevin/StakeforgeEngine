// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "data/bitmask.hpp"

namespace SFG
{
	struct resource_desc;

	class buffer
	{
	public:
		void create_staging_hw(const resource_desc& staging, const resource_desc& hw);
		void create_hw(const resource_desc& desc);
		void destroy();
		void buffer_data(size_t padding, const void* data, size_t size);
		bool copy(gfx_id cmd_buffer);

		inline gfx_id get_hw_staging() const
		{
			return _hw_staging;
		}
		inline gfx_id get_hw_gpu() const
		{
			return _hw_gpu;
		}

		inline bool is_dirty() const
		{
			return _flags.is_set(buffer_flags::buf_dirty);
		}

		inline bool is_alive() const
		{
			return _flags.is_set(buffer_flags::buf_alive);
		}

	private:
		enum buffer_flags
		{
			buf_has_staging = 1 << 0,
			buf_dirty		= 1 << 2,
			buf_alive		= 1 << 3,
		};

	private:
		uint8*		   _mapped	   = nullptr;
		gfx_id		   _hw_staging = 0;
		gfx_id		   _hw_gpu	   = 0;
		bitmask<uint8> _flags	   = 0;
	};
}

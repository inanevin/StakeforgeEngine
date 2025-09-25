// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/bitmask.hpp"
#include "data/static_vector.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/texture_buffer.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	struct texture_raw;

	class texture
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_texture;

		enum flags
		{
			hw_exists			= 1 << 0,
			intermediate_exists = 1 << 1,
		};

		~texture();

		void   create_from_raw(const texture_raw& raw);
		void   destroy_cpu();
		void   destroy();
		uint8  get_bpp() const;
		uint16 get_width() const;
		uint16 get_height() const;
		gfx_id get_hw() const;

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

		inline texture_buffer* get_cpu()
		{
			return &_cpu_buffers[0];
		}

		inline uint8 get_cpu_count() const
		{
			return static_cast<uint8>(_cpu_buffers.size());
		}

		inline gfx_id get_intermediate() const
		{
			return _intermediate;
		}

	private:
		void create_intermediate();
		void destroy_intermediate();

	private:
		friend struct texture_raw;
		static_vector<texture_buffer, MAX_TEXTURE_MIPS> _cpu_buffers;
		gfx_id											_hw			  = 0;
		gfx_id											_intermediate = 0;
		bitmask<uint8>									_flags		  = 0;
	};

}
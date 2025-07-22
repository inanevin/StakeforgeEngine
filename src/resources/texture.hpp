// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "gfx/common/format.hpp"
#include "gfx/common/gfx_common.hpp"

namespace Game
{
	class texture
	{
	private:
	public:
		~texture();
		// void copy(LinaGX::CommandStream* cmd);
		void destroy();

		void create_cpu(uint8* data, uint32 width, uint32 height, uint32 bbp, uint8 channels, bool linear);
		// void create_hw(const LinaGX::TextureDesc& desc);
		void create_hw_default();
		void destroy_cpu();
		void populate_mips_from_cpu(uint32 max_level = 0);

		inline void set_clear_buffers_after_copy(bool clear)
		{
			_clear_buffers_after_copy = clear;
		}

		inline uint8 get_bpp() const
		{
			return _cpu.empty() ? 0 : _cpu[0].bpp;
		}

		inline uint16 get_width() const
		{
			return _cpu.empty() ? 0 : _cpu[0].size.x;
		}

		inline uint16 get_height() const
		{
			return _cpu.empty() ? 0 : _cpu[0].size.y;
		}

	private:
		format determine_format_from_cpu();

	private:
		vector<texture_buffer> _cpu						 = {};
		uint32				   _hw						 = 0;
		uint8				   _mip_levels				 = 1;
		uint8				   _channels				 = 1;
		format				   _format					 = format::undefined;
		bool				   _is_linear				 = false;
		bool				   _owns_hw					 = false;
		bool				   _clear_buffers_after_copy = false;
	};

}
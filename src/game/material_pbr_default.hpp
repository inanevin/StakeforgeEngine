// Copyright (c) 2025 Inan Evin

#pragma once

#include "math/vector4.hpp"
#include "data/string_id.hpp"
#include "gfx/buffer.hpp"

namespace SFG
{

	class world_resources;

	class material_pbr_default
	{
	private:
		struct material_data
		{
			vector4 base_color = vector4::one;
		};

	public:
#ifdef SFG_TOOLMODE
		bool create_from_file(const char* file, world_resources& resources);
#endif

		void destroy();
		void update_material_data();

		inline bool is_dirty(uint8 frame_index) const
		{
			return _buffers[frame_index].is_dirty();
		}

		inline gfx_id get_gpu(uint8 frame_index) const
		{
			return _bind_groups[frame_index];
		}

		inline material_data& get_material_data()
		{
			return _material_data;
		}

	private:
		material_data _material_data				 = {};
		buffer		  _buffers[FRAMES_IN_FLIGHT]	 = {};
		gfx_id		  _bind_groups[FRAMES_IN_FLIGHT] = {};
	};

}

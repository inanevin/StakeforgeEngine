// Copyright (c) 2025 Inan Evin

#pragma once
#include "gfx/common/gfx_common.hpp"

namespace SFG
{

	struct constant_data
	{
		uint32 constant0;
		uint32 constant1;
		uint32 constant2;
		uint32 constant3;
	};

	struct indexed_draw
	{
		constant_data constants		 = {};
		uint32		  base_vertex	 = 0;
		uint32		  index_count	 = 0;
		uint32		  instance_count = 0;
		uint32		  start_index	 = 0;
		uint32		  start_instance = 0;
		gfx_id		  pipeline		 = 0;
		gfx_id		  bind_group	 = 0;
		gfx_id		  vertex_buffer	 = 0;
		gfx_id		  idx_buffer	 = 0;

		bool operator==(const indexed_draw& other)
		{
			return vertex_buffer == other.vertex_buffer && idx_buffer == other.idx_buffer && base_vertex == other.base_vertex && index_count == other.index_count && start_index == other.start_index && start_instance == other.start_instance &&
				   pipeline == other.pipeline && bind_group == other.bind_group;
		}
	};

}

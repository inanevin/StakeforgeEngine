// Copyright (c) 2025 Inan Evin
#pragma once

#include "data/bitmask.hpp"
#include "resource_limits.hpp"

namespace Game
{
	enum barrier_flags
	{
		baf_is_resource	 = 1 << 0,
		baf_is_texture	 = 1 << 1,
		baf_is_swapchain = 1 << 2,
	};

	enum class resource_state : uint8
	{
		common,
		vertex_cbv,
		index_buffer,
		render_target,
		uav,
		depth_write,
		depth_read,
		non_ps_resource,
		ps_resource,
		indirect_arg,
		copy_dest,
		copy_source,
		resolve_dest,
		resolve_source,
		generic_read,
		present,
	};

	struct barrier
	{
		resource_id	   resource	  = 0;
		bitmask<uint8> flags	  = 0;
		resource_state from_state = resource_state::common;
		resource_state to_state	  = resource_state::common;
	};

}
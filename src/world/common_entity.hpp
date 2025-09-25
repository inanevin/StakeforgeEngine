// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"
#include "memory/pool_handle.hpp"
#include "data/bitmask.hpp"
#undef max

namespace SFG
{
	typedef uint16		  world_id;
	typedef pool_handle16 entity_handle;

#define NULL_WORLD_ID std::numeric_limits<world_id>::max()

	enum entity_flags : uint8
	{
		entity_flags_local_transform_dirty = 1 << 0,
		entity_flags_abs_transform_dirty   = 1 << 1,
		entity_flags_abs_rotation_dirty	   = 1 << 2,
	};

	struct entity_meta
	{
		const char*		name  = "";
		bitmask<uint16> flags = entity_flags_local_transform_dirty | entity_flags_abs_transform_dirty;
	};

	struct entity_family
	{
		pool_handle16 parent	   = {};
		pool_handle16 first_child  = {};
		pool_handle16 prev_sibling = {};
		pool_handle16 next_sibling = {};
	};
}

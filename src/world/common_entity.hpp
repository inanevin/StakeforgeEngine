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
	struct entity_meta
	{
		const char*		name  = "";
		bitmask<uint16> flags = 0;
	};

	struct entity_family
	{
		pool_handle16 parent	   = {};
		pool_handle16 first_child  = {};
		pool_handle16 prev_sibling = {};
		pool_handle16 next_sibling = {};
	};
}

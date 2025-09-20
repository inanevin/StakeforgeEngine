// Copyright (c) 2025 Inan Evin
#pragma once
#include "world/common_world.hpp"
#include "common_trait.hpp"

namespace SFG
{
	class entity_manager;

	struct trait_light
	{
		pool_handle<world_id> entity = {};
		bitmask<uint8>		  flags	 = 0;

		static void on_add(entity_manager& em, trait_light& trait);
		static void on_remove(entity_manager& em, trait_light& trait);
	};
}

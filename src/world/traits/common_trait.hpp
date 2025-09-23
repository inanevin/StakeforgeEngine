// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"
#include "world/common_entity.hpp"
#include "data/bitmask.hpp"

namespace SFG
{
	enum trait_flags
	{
		trait_flags_is_disabled = 1 << 0,
		trait_flags_is_init		= 1 << 1,
	};

	enum trait_types : uint8
	{
		trait_type_mesh_renderer = 0,
		trait_type_light,
		trait_type_engine_max,
		trait_type_allowed_max = 32,
	};

	typedef pool_handle16 trait_handle;

	struct trait_meta
	{
		entity_handle  entity;
		bitmask<uint8> flags;
	};
}

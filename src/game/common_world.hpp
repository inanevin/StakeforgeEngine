// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"
#include "data/bitmask.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"

namespace SFG
{
	typedef uint16 entity_id;
	typedef uint16 resource_id;

#define MAX_ENTITIES 1024

	struct entity_meta
	{
		const char*		name  = "";
		bitmask<uint16> flags = 0;
	};

	struct entity_pos
	{
		vector3 position = vector3::zero;
	};

	struct entity_scale
	{
		vector3 scale = vector3::zero;
	};

	struct entity_rot
	{
		quat rot = quat::identity;
	};

}

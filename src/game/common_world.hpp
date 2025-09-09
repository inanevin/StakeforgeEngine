// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"
#include "data/bitmask.hpp"
#include "data/string.hpp"
#include "data/string_id.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "memory/pool_handle.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

namespace SFG
{
	typedef uint16 world_id;
	typedef uint16 resource_id;

	struct resource_ident
	{
#ifdef SFG_TOOLMODE
		string path = "";
#else
		string_id hash = 0;
#endif
	};

#define MAX_ENTITIES 512

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

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const resource_ident& s);
	void from_json(const nlohmann::json& j, resource_ident& s);

#endif

}

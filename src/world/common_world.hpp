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

#define MAX_WORLD_TEXTURES		 32
#define MAX_WORLD_MODELS		 32
#define MAX_WORLD_ANIMS			 32
#define MAX_WORLD_SKINS			 12
#define MAX_WORLD_MAT_PBR_DEF	 20
#define MAX_WORLD_SHADERS		 30
#define MAX_RENDERABLE_NODES	 1024
#define MAX_RENDERABLE_MATERIALS 256
#define MAX_ENTITIES			 512
#define MAX_TRAIT_AUX_MEMORY	 1024

	typedef uint16 world_id;
	typedef uint16 resource_id;

	struct resource_ident
	{
		string_id hash = 0;
	};

	struct entity_meta
	{
		const char*		name  = "";
		bitmask<uint16> flags = 0;
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const resource_ident& s);
	void from_json(const nlohmann::json& j, resource_ident& s);

#endif

}

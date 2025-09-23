// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"

namespace SFG
{

#define MAX_WORLD_TEXTURES		 32
#define MAX_WORLD_MODELS		 32
#define MAX_WORLD_MESHES		 256
#define MAX_WORLD_ANIMS			 32
#define MAX_WORLD_SKINS			 12
#define MAX_WORLD_MATERIALS		 20
#define MAX_WORLD_SHADERS		 30
#define MAX_RENDERABLE_NODES	 1024
#define MAX_RENDERABLE_MATERIALS 256
#define MAX_ENTITIES			 512
#define MAX_TRAIT_AUX_MEMORY	 1024
	static constexpr size_t MAX_MODEL_AUX_MEMORY = 1048576;

	typedef uint16 resource_id;

}

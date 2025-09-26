// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "memory/pool_handle.hpp"

namespace SFG
{

	enum resource_types : uint8
	{
		resource_type_texture = 0,
		resource_type_texture_sampler,
		resource_type_model,
		resource_type_animation,
		resource_type_skin,
		resource_type_material,
		resource_type_shader,
		resource_type_audio,
		resource_type_font,
		resource_type_mesh,
		resource_type_physical_material,
		resource_type_engine_max,
		resource_type_allowed_max = 16,
	};

	typedef pool_handle16 resource_handle;
}

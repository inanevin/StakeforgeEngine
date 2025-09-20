// Copyright (c) 2025 Inan Evin

#pragma once
#include "view_manager.hpp"
#include "gpu_bone.hpp"
#include "gpu_entity.hpp"
#include "gpu_light.hpp"
#include "renderable.hpp"

namespace SFG
{
#define MAX_RENDERABLES 1024

	struct world_render_data
	{
		view_manager									  views;
		static_vector<renderable_object, MAX_RENDERABLES> renderables;
		static_vector<gpu_entity, MAX_GPU_ENTITIES>		  entities;
		static_vector<gpu_light, MAX_GPU_LIGHTS>		  lights;
		static_vector<gpu_bone, MAX_GPU_BONES>			  bones;

		inline void reset()
		{
			views.reset();
			renderables.clear();
			entities.clear();
		}
	};
}

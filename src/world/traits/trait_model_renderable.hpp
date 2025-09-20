// Copyright (c) 2025 Inan Evin
#pragma once
#include "world/common_world.hpp"
#include "memory/chunk_handle.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "data/bitmask.hpp"
#include "common_trait.hpp"
#include "math/matrix4x3.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#endif

namespace SFG
{
	class entity_manager;
	class world;
	class material;

	struct trait_model_renderable_node
	{
		matrix4x3 localMatrix;
	};

#ifdef SFG_TOOLMODE

	struct trait_model_renderable_json
	{
		string		   target_model = "";
		vector<string> materials	= {};
		world_id	   entity		= 0;
		bitmask<uint8> flags;
	};

	void to_json(nlohmann::json& j, const trait_model_renderable_json& trait);
	void from_json(const nlohmann::json& j, trait_model_renderable_json& trait);

#endif

	struct trait_model_renderable
	{
		pool_handle<world_id> entity	   = {};
		pool_handle<world_id> target_model = {};
		chunk_handle<uint16>  nodes		   = {};
		chunk_handle<uint16>  materials	   = {};
		bitmask<uint8>		  flags		   = {};

		void						 set_model(world* world, string_id hash);
		void						 set_material(world* world, uint8 index, string_id hash);
		trait_model_renderable_node* get_node(world* world, uint16 node_index);
		material&					 get_material(world* world, uint16 material_index);
		pool_handle<resource_id>	 get_material_handle(world* world, uint16 material_index);

		static void on_add(entity_manager& em, trait_model_renderable& trait);
		static void on_remove(entity_manager& em, trait_model_renderable& trait);
	};

}

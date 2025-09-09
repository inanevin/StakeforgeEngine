// Copyright (c) 2025 Inan Evin
#pragma once
#include "game/common_world.hpp"
#include "data/bitmask.hpp"
#include "common_trait.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

namespace SFG
{
	class entity_manager;

	struct trait_model_renderable
	{
		pool_handle<world_id> entity	   = {};
		bitmask<uint8>		  flags		   = {};
		resource_ident		  target_model = {};

		static void on_add(entity_manager& em, trait_model_renderable& trait);
		static void on_remove(entity_manager& em, trait_model_renderable& trait);
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const trait_model_renderable& trait);
	void from_json(const nlohmann::json& j, trait_model_renderable& trait);

#endif

}

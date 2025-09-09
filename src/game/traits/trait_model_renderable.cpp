// Copyright (c) 2025 Inan Evin

#include "trait_model_renderable.hpp"
#include "game/entity_manager.hpp"
#include "game/world.hpp"
#include "resources/model.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif

namespace SFG
{
	void trait_model_renderable::on_add(entity_manager& em, trait_model_renderable& trait)
	{
		world& w = em.get_world();

#ifdef SFG_TOOLMODE
		const string_id hash = TO_SID(trait.target_model.path);
#else
		const string_id hash = trait.target_model.hash;
#endif

		model& mdl		   = w.get_resources().get_model_by_hash(hash);
		aabb&  entity_aabb = em.get_entity_aabb(trait.entity);
		entity_aabb.add(mdl.get_total_aabb());
	}

	void trait_model_renderable::on_remove(entity_manager& em, trait_model_renderable& trait)
	{
		world& w = em.get_world();

#ifdef SFG_TOOLMODE
		const string_id hash = TO_SID(trait.target_model.path);
#else
		const string_id hash = trait.target_model.hash;
#endif
		model& mdl = w.get_resources().get_model_by_hash(hash);

		aabb& entity_aabb = em.get_entity_aabb(trait.entity);
		entity_aabb.remove(mdl.get_total_aabb());
	}

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const trait_model_renderable& trait)
	{
		j["target_model"] = trait.target_model;
		j["entity"]		  = trait.entity.id;
		j["flags"]		  = trait.flags.value();
	}

	void from_json(const nlohmann::json& j, trait_model_renderable& trait)
	{
		trait.target_model = j.value<resource_ident>("target_model", {});
		trait.entity.id	   = j.value<world_id>("entity", 0);
		trait.flags		   = j.value<uint8>("flags", 0);
	}
#endif
}
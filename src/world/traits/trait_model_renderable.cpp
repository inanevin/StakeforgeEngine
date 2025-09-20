// Copyright (c) 2025 Inan Evin

#include "trait_model_renderable.hpp"
#include "world/entity_manager.hpp"
#include "world/world.hpp"
#include "resources/model.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif

namespace SFG
{
	void trait_model_renderable::set_model(world* world, string_id hash)
	{
		entity_manager& em		  = world->get_entity_manager();
		auto&			trait_aux = em.get_trait_aux_memory();

		if (flags.is_set(trait_flags_is_init))
		{
			trait_aux.free(nodes);
			trait_aux.free(materials);
		}

		world_resources& res = world->get_resources();

		target_model = res.get_model_handle_by_hash(hash);
		model& mdl	 = res.get_model(target_model);

		const vector<model_node>& mdl_nodes	 = mdl.get_nodes();
		const world_id			  node_count = static_cast<world_id>(mdl_nodes.size());
		nodes								 = trait_aux.allocate(node_count * sizeof(trait_model_renderable_node));

		for (world_id i = 0; i < node_count; i++)
		{
			const model_node&			 mdl_node = mdl_nodes[i];
			trait_model_renderable_node* node	  = reinterpret_cast<trait_model_renderable_node*>(trait_aux.get(nodes.head) + i * sizeof(trait_model_renderable_node));
			node->localMatrix					  = mdl_node.local_matrix;
		}

		const uint8 mdl_material_count = mdl.get_material_count();
		materials					   = trait_aux.allocate(mdl_material_count * sizeof(pool_handle<resource_id>));

		flags.set(trait_flags_is_init);
	}

	void trait_model_renderable::set_material(world* world, uint8 index, string_id hash)
	{
		SFG_ASSERT(index < materials.size / sizeof(string_id));
		entity_manager&			  em		= world->get_entity_manager();
		auto&					  trait_aux = em.get_trait_aux_memory();
		pool_handle<resource_id>* mat		= reinterpret_cast<pool_handle<resource_id>*>(trait_aux.get(materials.head) + index * sizeof(pool_handle<resource_id>));
		*mat								= world->get_resources().get_material_handle_by_hash(hash);
	}

	trait_model_renderable_node* trait_model_renderable::get_node(world* world, uint16 node_index)
	{
		entity_manager& em		  = world->get_entity_manager();
		auto&			trait_aux = em.get_trait_aux_memory();
		return reinterpret_cast<trait_model_renderable_node*>(trait_aux.get(nodes.head + sizeof(trait_model_renderable_node) * node_index));
	}

	material& trait_model_renderable::get_material(world* world, uint16 material_index)
	{
		entity_manager&			  em		= world->get_entity_manager();
		auto&					  trait_aux = em.get_trait_aux_memory();
		pool_handle<resource_id>* handle	= reinterpret_cast<pool_handle<resource_id>*>(trait_aux.get(materials.head + sizeof(pool_handle<resource_id>) * material_index));
		return world->get_resources().get_material(*handle);
	}

	pool_handle<resource_id> trait_model_renderable::get_material_handle(world* world, uint16 material_index)
	{
		entity_manager& em		  = world->get_entity_manager();
		auto&			trait_aux = em.get_trait_aux_memory();
		return *(reinterpret_cast<pool_handle<resource_id>*>(trait_aux.get(materials.head + sizeof(pool_handle<resource_id>) * material_index)));
	}

	void trait_model_renderable::on_add(entity_manager& em, trait_model_renderable& trait)
	{
		world&			 w				 = em.get_world();
		world_resources& world_resources = w.get_resources();
		model&			 mdl			 = world_resources.get_model(trait.target_model);

		aabb& entity_aabb = em.get_entity_aabb(trait.entity);
		entity_aabb.add(mdl.get_total_aabb());
	}

	void trait_model_renderable::on_remove(entity_manager& em, trait_model_renderable& trait)
	{
		world& w		 = em.get_world();
		auto&  trait_aux = em.get_trait_aux_memory();

		if (trait.flags.is_set(trait_flags_is_init))
		{
			trait_aux.free(trait.nodes);
			trait_aux.free(trait.materials);
		}

		model& mdl		   = w.get_resources().get_model(trait.target_model);
		aabb&  entity_aabb = em.get_entity_aabb(trait.entity);
		entity_aabb.remove(mdl.get_total_aabb());
		trait.flags.remove(trait_flags_is_init);
	}

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const trait_model_renderable_json& trait)
	{
		j["target_model"] = trait.target_model;
		j["materials"]	  = trait.materials;
		j["entity"]		  = trait.entity;
		j["flags"]		  = trait.flags.value();
	}

	void from_json(const nlohmann::json& j, trait_model_renderable_json& trait)
	{
		trait.target_model = j.value<string>("target_model", "");
		trait.materials	   = j.value<vector<string>>("materials", {});
		trait.entity	   = j.value<world_id>("entity", 0);
		trait.flags		   = j.value<uint8>("flags", 0);
	}
#endif
}
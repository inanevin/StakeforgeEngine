// Copyright (c) 2025 Inan Evin

#pragma once
#include "common_world.hpp"
#include "memory/pool_allocator.hpp"
#include "math/aabb.hpp"

#include "traits/trait_model_renderable.hpp"
#include "traits/trait_view.hpp"
#include "traits/trait_light.hpp"
#include <tuple>

namespace SFG
{
	class world;

	// clang-format off
#define DEFINE_TRAIT_FUNCS(TRAITNAME, VIEWNAME)                             \
	pool_handle<world_id> add_##TRAITNAME(pool_handle<world_id> entity, ##TRAITNAME initial = {})     \
	{                                                                       \
		auto&				  pool	 = ##VIEWNAME.get_pool();               \
		pool_handle<world_id> handle = pool.add();                          \
		auto&				  trait	 = pool.get(handle);                    \
		trait = initial;													\
		trait.entity				 = entity;                              \
		##TRAITNAME::on_add(*this, trait);                                  \
		return handle;                                                      \
	}																		\
	void remove_##TRAITNAME(pool_handle<world_id> handle)                   \
	{                                                                       \
		auto& pool	= ##VIEWNAME.get_pool();                                \
		auto& trait = pool.get(handle);                                     \
		##TRAITNAME::on_remove(*this, trait);                               \
		pool.remove(handle);												\
	}																		\
	##TRAITNAME& get_##TRAITNAME(pool_handle<world_id> handle)              \
	{																		\
		auto& pool = ##VIEWNAME.get_pool();										\
		return  pool.get(handle);												\
	}

	// clang-format on

	class entity_manager
	{
	public:
		entity_manager() = delete;
		entity_manager(world& w) : _world(w){};

		void init();
		void uninit();

		pool_handle<world_id> create_entity(const char* name = "entity");
		void				  destroy_entity(pool_handle<world_id> handle);
		aabb&				  get_entity_aabb(pool_handle<world_id> entity);

		inline world& get_world()
		{
			return _world;
		}

		DEFINE_TRAIT_FUNCS(trait_model_renderable, _trait_renderables);
		DEFINE_TRAIT_FUNCS(trait_light, _trait_lights);

	private:
		world& _world;

		pool_allocator_gen<world_id, world_id, MAX_ENTITIES> _entities = {};

		pool_allocator_simple<entity_meta, world_id, MAX_ENTITIES>	_metas	   = {};
		pool_allocator_simple<entity_pos, world_id, MAX_ENTITIES>	_positions = {};
		pool_allocator_simple<entity_rot, world_id, MAX_ENTITIES>	_rotations = {};
		pool_allocator_simple<entity_scale, world_id, MAX_ENTITIES> _scales	   = {};
		pool_allocator_simple<aabb, world_id, MAX_ENTITIES>			_aabbs	   = {};

		trait_view<trait_model_renderable, 256> _trait_renderables;
		trait_view<trait_light, 100>			_trait_lights;
	};
}

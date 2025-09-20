// Copyright (c) 2025 Inan Evin

#pragma once
#include "common_world.hpp"
#include "memory/pool_allocator.hpp"
#include "memory/chunk_allocator.hpp"
#include "math/aabb.hpp"
#include "math/matrix4x3.hpp"

#include "world/traits/trait_model_renderable.hpp"
#include "world/traits/trait_view.hpp"
#include "world/traits/trait_light.hpp"
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
		auto& pool = ##VIEWNAME.get_pool();									\
		return pool.get(handle);											\
	}																		\
	inline auto& get_view_##TRAITNAME()											\
	{																		\
		auto& view = ##VIEWNAME;											\
		return view;														\
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

		const vector3&	 get_entity_position_abs(pool_handle<world_id> entity) const;
		const vector3&	 get_entity_prev_position_abs(pool_handle<world_id> entity) const;
		void			 set_entity_position_abs(pool_handle<world_id> entity, const vector3& pos);
		void			 set_entity_prev_position_abs(pool_handle<world_id> entity, const vector3& pos);
		const quat&		 get_entity_rotation_abs(pool_handle<world_id> entity) const;
		const quat&		 get_entity_prev_rotation_abs(pool_handle<world_id> entity) const;
		void			 set_entity_rotation_abs(pool_handle<world_id> entity, const quat& rot);
		void			 set_entity_prev_rotation_abs(pool_handle<world_id> entity, const quat& rot);
		const vector3&	 get_entity_scale_abs(pool_handle<world_id> entity) const;
		const vector3&	 get_entity_prev_scale_abs(pool_handle<world_id> entity) const;
		void			 set_entity_scale_abs(pool_handle<world_id> entity, const vector3& scale);
		void			 set_entity_prev_scale_abs(pool_handle<world_id> entity, const vector3& scale);
		const matrix4x3& get_entity_matrix(pool_handle<world_id> entity) const;
		const matrix4x3& get_entity_prev_matrix(pool_handle<world_id> entity) const;
		void			 set_entity_matrix(pool_handle<world_id> entity, const matrix4x3& mat);
		void			 set_entity_prev_matrix(pool_handle<world_id> entity, const matrix4x3& mat);

		inline world& get_world()
		{
			return _world;
		}

		inline chunk_allocator16<MAX_TRAIT_AUX_MEMORY>& get_trait_aux_memory()
		{
			return _trait_aux_memory;
		}

		inline auto& get_entities()
		{
			return _entities;
		}

		DEFINE_TRAIT_FUNCS(trait_model_renderable, _trait_renderables);
		DEFINE_TRAIT_FUNCS(trait_light, _trait_lights);

	private:
		void reset_all_entity_data();
		void reset_entity_data(world_id id);

	private:
		world& _world;

		pool_allocator_gen<world_id, world_id, MAX_ENTITIES> _entities = {};

		pool_allocator_simple<entity_meta, world_id, MAX_ENTITIES> _metas		   = {};
		pool_allocator_simple<vector3, world_id, MAX_ENTITIES>	   _positions	   = {};
		pool_allocator_simple<vector3, world_id, MAX_ENTITIES>	   _prev_positions = {};
		pool_allocator_simple<quat, world_id, MAX_ENTITIES>		   _rotations	   = {};
		pool_allocator_simple<quat, world_id, MAX_ENTITIES>		   _prev_rotations = {};
		pool_allocator_simple<vector3, world_id, MAX_ENTITIES>	   _scales		   = {};
		pool_allocator_simple<vector3, world_id, MAX_ENTITIES>	   _prev_scales	   = {};
		pool_allocator_simple<aabb, world_id, MAX_ENTITIES>		   _aabbs		   = {};
		pool_allocator_simple<matrix4x3, world_id, MAX_ENTITIES>   _matrices	   = {};
		pool_allocator_simple<matrix4x3, world_id, MAX_ENTITIES>   _prev_matrices  = {};

		trait_view<trait_model_renderable, 256> _trait_renderables;
		trait_view<trait_light, 100>			_trait_lights;

		chunk_allocator16<MAX_TRAIT_AUX_MEMORY> _trait_aux_memory;
	};
}

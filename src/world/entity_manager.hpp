// Copyright (c) 2025 Inan Evin

#pragma once
#include "common_world.hpp"
#include "common_entity.hpp"
#include "traits/common_trait.hpp"
#include "memory/pool_allocator_simple.hpp"
#include "memory/pool_allocator16.hpp"
#include "data/static_vector.hpp"
#include "memory/chunk_allocator.hpp"
#include "math/aabb.hpp"
#include "math/matrix4x3.hpp"
#include "math/quat.hpp"
#include <gui/vekt.hpp>
#include <functional>

namespace SFG
{
	class world;

	struct trait_storage
	{
		pool_allocator16 storage;
	};

	class entity_manager
	{
	public:
		entity_manager() = delete;
		entity_manager(world& w);
		~entity_manager();

		void init();
		void uninit();

		/* ---------------- entity api ---------------- */
		entity_handle		 create_entity(const char* name = "entity");
		void				 destroy_entity(entity_handle handle);
		void				 add_child(entity_handle parent, entity_handle child);
		void				 remove_child(entity_handle parent, entity_handle child);
		void				 remove_from_parent(entity_handle entity);
		entity_handle		 get_child_by_index(entity_handle parent, uint32 index);
		const aabb&			 get_entity_aabb(entity_handle entity);
		const entity_meta&	 get_entity_meta(entity_handle entity) const;
		const entity_family& get_entity_family(entity_handle entity) const;

		/* ---------------- entity transforms ---------------- */
		void			 set_entity_position(entity_handle entity, const vector3& pos);
		void			 set_entity_position_abs(entity_handle entity, const vector3& pos);
		const vector3&	 get_entity_position(entity_handle entity) const;
		vector3			 get_entity_position_abs(entity_handle entity);
		void			 set_entity_rotation(entity_handle entity, const quat& rot);
		void			 set_entity_rotation_abs(entity_handle entity, const quat& rot);
		const quat&		 get_entity_rotation(entity_handle entity) const;
		const quat&		 get_entity_rotation_abs(entity_handle entity);
		void			 set_entity_scale(entity_handle entity, const vector3& scale);
		void			 set_entity_scale_abs(entity_handle entity, const vector3& scale);
		const vector3&	 get_entity_scale(entity_handle entity) const;
		vector3			 get_entity_scale_abs(entity_handle entity);
		const matrix4x3& get_entity_transform(entity_handle entity);
		const matrix4x3& get_entity_transform_abs(entity_handle entity);
		void			 set_entity_prev_position_abs(entity_handle entity, const vector3& pos);
		void			 set_entity_prev_rotation_abs(entity_handle entity, const quat& rot);
		void			 set_entity_prev_scale_abs(entity_handle entity, const vector3& scale);
		const vector3&	 get_entity_prev_position_abs(entity_handle entity) const;
		const quat&		 get_entity_prev_rotation_abs(entity_handle entity) const;
		const vector3&	 get_entity_prev_scale_abs(entity_handle entity) const;
		matrix4x3		 calculate_interpolated_transform_abs(entity_handle entity, float interpolation);

		template <typename VisitFunc> void visit_children(entity_handle parent, VisitFunc f)
		{
			const entity_family& fam	= get_entity_family(parent);
			entity_handle		 entity = fam.first_child;

			while (!entity.is_null())
			{
				f(entity);
				visit_children(entity, f);
				entity = get_entity_family(entity).next_sibling;
			}
		}

		template <typename VisitFunc> void visit_parents(entity_handle entity, VisitFunc f)
		{
			const entity_family& fam	= get_entity_family(entity);
			entity_handle		 parent = fam.parent;
			while (!parent.is_null())
			{
				f(parent);
				visit_parents(parent, f);
				parent = get_entity_family(parent).parent;
			}
		}

		/* ---------------- trait api ---------------- */

		template <typename T> void init_trait_storage(uint32 max_count)
		{
			_traits[T::TYPE_INDEX].storage.init<T>(max_count);
		}

		template <typename T> trait_handle add_trait(entity_handle entity)
		{
			pool_allocator16& storage = _traits[T::TYPE_INDEX].storage;
			trait_handle	  handle  = storage.allocate<T>();
			T&				  tr	  = storage.get<T>(handle);
			tr						  = T();
			tr.meta.entity			  = entity;
			return handle;
		}

		template <typename T> T& get_trait(trait_handle handle)
		{
			pool_allocator16& storage = _traits[T::TYPE_INDEX].storage;
			return storage.get<T>(handle);
		}
		template <typename T> const pool_allocator16& get_trait_storage() const
		{
			const pool_allocator16& storage = _traits[T::TYPE_INDEX].storage;
			return storage;
		}

		template <typename T> void remove_trait(trait_handle handle)
		{
			pool_allocator16& storage = _traits[T::TYPE_INDEX].storage;
			T&				  tr	  = storage.get<T>(handle);
			tr.~T();
			storage.free(handle);
		}

		inline chunk_allocator32& get_trait_aux_memory()
		{
			return _trait_aux_memory;
		}

		inline static_vector<trait_storage, trait_types::trait_type_allowed_max>& get_traits()
		{
			return _traits;
		}

		/* ---------------- rest ---------------- */

		inline world& get_world()
		{
			return _world;
		}

		inline pool_allocator16& get_entities()
		{
			return _entities;
		}

	private:
		void reset_all_entity_data();
		void reset_entity_data(world_id id);

	private:
		world& _world;

		pool_allocator16					 _entities		 = {};
		pool_allocator_simple<entity_meta>	 _metas			 = {};
		pool_allocator_simple<entity_family> _families		 = {};
		pool_allocator_simple<vector3>		 _positions		 = {};
		pool_allocator_simple<vector3>		 _prev_positions = {};
		pool_allocator_simple<quat>			 _rotations		 = {};
		pool_allocator_simple<quat>			 _rotations_abs	 = {};
		pool_allocator_simple<quat>			 _prev_rotations = {};
		pool_allocator_simple<vector3>		 _scales		 = {};
		pool_allocator_simple<vector3>		 _prev_scales	 = {};
		pool_allocator_simple<aabb>			 _aabbs			 = {};
		pool_allocator_simple<matrix4x3>	 _matrices		 = {};
		pool_allocator_simple<matrix4x3>	 _abs_matrices	 = {};

		static_vector<trait_storage, trait_types::trait_type_allowed_max> _traits;
		chunk_allocator32												  _trait_aux_memory;
	};
}

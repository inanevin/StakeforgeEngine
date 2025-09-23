// Copyright (c) 2025 Inan Evin

#include "entity_manager.hpp"
#include "world.hpp"
#include "world/traits/trait_mesh_renderer.hpp"
#include "world/traits/trait_light.hpp"

namespace SFG
{
	entity_manager::entity_manager(world& w) : _world(w)
	{
		_entities.init<world_id>(MAX_ENTITIES);
		_metas.init(MAX_ENTITIES);
		_positions.init(MAX_ENTITIES);
		_prev_positions.init(MAX_ENTITIES);
		_rotations.init(MAX_ENTITIES);
		_prev_rotations.init(MAX_ENTITIES);
		_scales.init(MAX_ENTITIES);
		_prev_scales.init(MAX_ENTITIES);
		_aabbs.init(MAX_ENTITIES);
		_matrices.init(MAX_ENTITIES);
		_prev_matrices.init(MAX_ENTITIES);
		_families.init(MAX_ENTITIES);

		_traits.resize(1);
		_traits[trait_light::TYPE_INDEX].storage.init<trait_light>(100);
	}

	entity_manager::~entity_manager()
	{
		for (trait_storage& stg : _traits)
			stg.storage.uninit();
		_entities.uninit();
	}

	void entity_manager::init()
	{
	}

	void entity_manager::uninit()
	{
		for (trait_storage& stg : _traits)
			stg.storage.reset();

		reset_all_entity_data();
	}

	entity_handle entity_manager::create_entity(const char* name)
	{
		entity_handle handle = _entities.allocate<world_id>();
		set_entity_scale_abs(handle, vector3::one);
		set_entity_prev_scale_abs(handle, vector3::one);

		entity_meta& meta = _metas.get(handle.index);
		meta.name		  = _world.get_text_allocator().allocate(name);

		return handle;
	}

	void entity_manager::destroy_entity(entity_handle entity)
	{
		SFG_ASSERT(_entities.is_valid(entity));

		entity_meta& meta = _metas.get(entity.index);
		_world.get_text_allocator().deallocate((char*)meta.name);

		entity_family& fam = _families.get(entity.index);

		// If parent exists, we gotta check for first-child case.
		if (!fam.parent.is_null())
		{
			SFG_ASSERT(_entities.is_valid(fam.parent));
			entity_family& fam_parent = _families.get(fam.parent.index);

			if (fam_parent.first_child == entity)
			{
				// No more first child, and we assign the next sibling if needed.
				fam_parent.first_child = {};

				if (!fam.next_sibling.is_null())
				{
					SFG_ASSERT(_entities.is_valid(fam.next_sibling));
					fam_parent.first_child = fam.next_sibling;
				}
			}
		}

		// Assign next/prev siblings
		if (!fam.prev_sibling.is_null())
		{
			SFG_ASSERT(_entities.is_valid(fam.prev_sibling));
			entity_family& fam_prev = _families.get(fam.prev_sibling.index);
			fam_prev.next_sibling	= fam.next_sibling;
		}

		if (!fam.next_sibling.is_null())
		{
			SFG_ASSERT(_entities.is_valid(fam.next_sibling));
			entity_family& fam_next = _families.get(fam.next_sibling.index);
			fam_next.prev_sibling	= fam.prev_sibling;
		}

		// Destroy children recursively.
		entity_handle target_child = fam.first_child;
		while (!target_child.is_null())
		{
			entity_handle next = get_entity_family(target_child).next_sibling;
			destroy_entity(target_child);
			target_child = next;
		}

		reset_entity_data(entity.index);
		_entities.free<world_id>(entity);
	}

	aabb& entity_manager::get_entity_aabb(entity_handle entity)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _aabbs.get(entity.index);
	}

	void entity_manager::add_child(entity_handle parent, entity_handle child_to_add)
	{
		SFG_ASSERT(_entities.is_valid(parent));
		SFG_ASSERT(_entities.is_valid(child_to_add));
		entity_family& fam_parent = _families.get(parent.index);
		entity_family& fam_child  = _families.get(child_to_add.index);

		// remove from current parent if exists
		if (_entities.is_valid(fam_child.parent))
			remove_child(fam_child.parent, child_to_add);

		fam_child.parent = parent;

		// If first child just add.
		if (fam_parent.first_child.is_null())
		{
			fam_parent.first_child = child_to_add;
			fam_child.prev_sibling = fam_child.next_sibling = {};
		}
		else
		{
			SFG_ASSERT(_entities.is_valid(fam_parent.first_child));
			entity_family& fam_first_child = _families.get(fam_parent.first_child.index);

			// We gotta find the last child. Start with the first one, iterate until next one is null.
			entity_handle last_child = fam_parent.first_child;
			entity_handle next_child = last_child;
			while (!next_child.is_null())
			{
				SFG_ASSERT(_entities.is_valid(next_child));
				entity_family& fam_current = _families.get(next_child.index);
				last_child				   = next_child;
				next_child				   = fam_current.next_sibling;
			}

			SFG_ASSERT(!last_child.is_null());
			SFG_ASSERT(_entities.is_valid(last_child));
			entity_family& fam_found_last_child = _families.get(last_child.index);
			fam_found_last_child.next_sibling	= child_to_add;
			fam_child.prev_sibling				= last_child;
		}
	}

	void entity_manager::remove_child(entity_handle parent, entity_handle child_to_remove)
	{
		SFG_ASSERT(_entities.is_valid(parent));
		SFG_ASSERT(_entities.is_valid(child_to_remove));

		entity_family& fam_parent = _families.get(parent.index);
		entity_family& fam_child  = _families.get(child_to_remove.index);

		if (fam_parent.first_child == child_to_remove)
		{
			// invalidate first child.
			// we assign next sibling as first if applicable.
			fam_parent.first_child = {};

			if (!fam_child.next_sibling.is_null())
			{
				SFG_ASSERT(_entities.is_valid(fam_child.next_sibling));

				fam_parent.first_child = fam_child.next_sibling;

				// also invalidate prev sibling as it was assigned as first.
				entity_family& fam_next_sibling = _families.get(fam_child.next_sibling.index);
				fam_next_sibling.prev_sibling	= {};
			}
		}
		else
		{
			// If not the first, there must be a prev sibling.
			// Find it, give it our current next sibling.
			SFG_ASSERT(!fam_child.prev_sibling.is_null());
			SFG_ASSERT(_entities.is_valid(fam_child.prev_sibling));

			entity_family& fam_prev_sibling = _families.get(fam_child.prev_sibling.index);
			fam_prev_sibling.next_sibling	= fam_child.next_sibling;

			// If our current next sibling is alive, give it our current prev sibling.
			if (!fam_child.next_sibling.is_null())
			{
				SFG_ASSERT(_entities.is_valid(fam_child.next_sibling));
				entity_family& fam_next_sibling = _families.get(fam_child.next_sibling.index);
				fam_next_sibling.prev_sibling	= fam_child.prev_sibling;
			}
		}

		fam_child.next_sibling = {};
		fam_child.prev_sibling = {};
		fam_child.parent	   = {};
	}

	void entity_manager::remove_from_parent(entity_handle entity)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		const entity_family& family = _families.get(entity.index);
		SFG_ASSERT(_entities.is_valid(family.parent));
		remove_child(family.parent, entity);
	}

	entity_handle entity_manager::get_child_by_index(entity_handle entity, uint32 index)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		const entity_family& family = _families.get(entity.index);

		if (family.first_child.is_null())
			return {};

		entity_handle target = family.first_child;

		for (uint32 i = 0; i < index; i++)
		{
			entity_family& fam	= _families.get(target.index);
			entity_handle  next = fam.next_sibling;
			if (next.is_null())
				break;
			target = next;
		}

		return target;
	}

	const entity_meta& entity_manager::get_entity_meta(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _metas.get(entity.index);
	}

	const entity_family& entity_manager::get_entity_family(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _families.get(entity.index);
	}

	const vector3& entity_manager::get_entity_position_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _positions.get(entity.index);
	}

	const vector3& entity_manager::get_entity_prev_position_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_positions.get(entity.index);
	}

	void entity_manager::set_entity_position_abs(entity_handle entity, const vector3& pos)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_positions.get(entity.index) = pos;
	}

	void entity_manager::set_entity_prev_position_abs(entity_handle entity, const vector3& pos)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_positions.get(entity.index) = pos;
	}

	const quat& entity_manager::get_entity_rotation_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _rotations.get(entity.index);
	}

	const quat& entity_manager::get_entity_prev_rotation_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_rotations.get(entity.index);
	}

	void entity_manager::set_entity_rotation_abs(entity_handle entity, const quat& rot)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_rotations.get(entity.index) = rot;
	}

	void entity_manager::set_entity_prev_rotation_abs(entity_handle entity, const quat& rot)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_rotations.get(entity.index) = rot;
	}

	const vector3& entity_manager::get_entity_scale_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _scales.get(entity.index);
	}

	const vector3& entity_manager::get_entity_prev_scale_abs(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_scales.get(entity.index);
	}

	void entity_manager::set_entity_scale_abs(entity_handle entity, const vector3& scale)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_scales.get(entity.index) = scale;
	}

	void entity_manager::set_entity_prev_scale_abs(entity_handle entity, const vector3& scale)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_scales.get(entity.index) = scale;
	}

	const matrix4x3& entity_manager::get_entity_matrix(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _matrices.get(entity.index);
	}

	const matrix4x3& entity_manager::get_entity_prev_matrix(entity_handle entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_matrices.get(entity.index);
	}

	void entity_manager::set_entity_matrix(entity_handle entity, const matrix4x3& mat)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_matrices.get(entity.index) = mat;
	}

	void entity_manager::set_entity_prev_matrix(entity_handle entity, const matrix4x3& mat)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_matrices.get(entity.index) = mat;
	}

	void entity_manager::reset_all_entity_data()
	{
		_entities.reset();
		_aabbs.reset();
		_metas.reset();
		_positions.reset();
		_prev_positions.reset();
		_rotations.reset();
		_prev_rotations.reset();
		_scales.reset();
		_prev_scales.reset();
		_matrices.reset();
		_prev_matrices.reset();
		_families.reset();
	}

	void entity_manager::reset_entity_data(world_id id)
	{
		_aabbs.reset(id);
		_metas.reset(id);
		_positions.reset(id);
		_prev_positions.reset(id);
		_rotations.reset(id);
		_prev_rotations.reset(id);
		_scales.reset(id);
		_prev_scales.reset(id);
		_matrices.reset(id);
		_prev_matrices.reset(id);
		_families.reset(id);
	}

}
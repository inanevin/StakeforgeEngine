// Copyright (c) 2025 Inan Evin

#include "entity_manager.hpp"
#include "world.hpp"

namespace SFG
{

	void entity_manager::init()
	{
	}

	void entity_manager::uninit()
	{
		reset_all_entity_data();
	}

	pool_handle<world_id> entity_manager::create_entity(const char* name)
	{
		pool_handle<world_id> handle = _entities.add();

		set_entity_scale_abs(handle, vector3::one);
		set_entity_prev_scale_abs(handle, vector3::one);

		return handle;
	}

	void entity_manager::destroy_entity(pool_handle<world_id> handle)
	{
		_entities.remove(handle);
		entity_meta& meta = _metas.get(handle.id);

		_world.get_text_allocator().deallocate((char*)meta.name);

		reset_entity_data(handle.id);
	}

	aabb& entity_manager::get_entity_aabb(pool_handle<world_id> entity)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _aabbs.get(entity.id);
	}

	const vector3& entity_manager::get_entity_position_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _positions.get(entity.id);
	}

	const vector3& entity_manager::get_entity_prev_position_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_positions.get(entity.id);
	}

	void entity_manager::set_entity_position_abs(pool_handle<world_id> entity, const vector3& pos)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_positions.get(entity.id) = pos;
	}

	void entity_manager::set_entity_prev_position_abs(pool_handle<world_id> entity, const vector3& pos)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_positions.get(entity.id) = pos;
	}

	const quat& entity_manager::get_entity_rotation_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _rotations.get(entity.id);
	}

	const quat& entity_manager::get_entity_prev_rotation_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_rotations.get(entity.id);
	}

	void entity_manager::set_entity_rotation_abs(pool_handle<world_id> entity, const quat& rot)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_rotations.get(entity.id) = rot;
	}

	void entity_manager::set_entity_prev_rotation_abs(pool_handle<world_id> entity, const quat& rot)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_rotations.get(entity.id) = rot;
	}

	const vector3& entity_manager::get_entity_scale_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _scales.get(entity.id);
	}

	const vector3& entity_manager::get_entity_prev_scale_abs(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_scales.get(entity.id);
	}

	void entity_manager::set_entity_scale_abs(pool_handle<world_id> entity, const vector3& scale)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_scales.get(entity.id) = scale;
	}

	void entity_manager::set_entity_prev_scale_abs(pool_handle<world_id> entity, const vector3& scale)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_scales.get(entity.id) = scale;
	}

	const matrix4x3& entity_manager::get_entity_matrix(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _matrices.get(entity.id);
	}

	const matrix4x3& entity_manager::get_entity_prev_matrix(pool_handle<world_id> entity) const
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _prev_matrices.get(entity.id);
	}

	void entity_manager::set_entity_matrix(pool_handle<world_id> entity, const matrix4x3& mat)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_matrices.get(entity.id) = mat;
	}

	void entity_manager::set_entity_prev_matrix(pool_handle<world_id> entity, const matrix4x3& mat)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		_prev_matrices.get(entity.id) = mat;
	}

	void entity_manager::reset_all_entity_data()
	{
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
	}

}
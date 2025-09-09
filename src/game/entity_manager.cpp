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
		_metas.reset();
		_positions.reset();
		_rotations.reset();
		_scales.reset();
	}

	pool_handle<world_id> entity_manager::create_entity(const char* name)
	{
		pool_handle<world_id> handle = _entities.add();
		return handle;
	}

	void entity_manager::destroy_entity(pool_handle<world_id> handle)
	{

		_entities.remove(handle);
		entity_meta& meta = _metas.get(handle.id);

		_world.get_text_allocator().deallocate((char*)meta.name);

		_aabbs.reset(handle.id);
		_metas.reset(handle.id);
		_positions.reset(handle.id);
		_rotations.reset(handle.id);
		_scales.reset(handle.id);
	}

	aabb& entity_manager::get_entity_aabb(pool_handle<world_id> entity)
	{
		SFG_ASSERT(_entities.is_valid(entity));
		return _aabbs.get(entity.id);
	}

}
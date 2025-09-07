// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/bitmask.hpp"
#include "memory/pool_allocator.hpp"
#include "common_world.hpp"
#include "data/vector.hpp"
#include "memory/text_allocator.hpp"
#include "world_resources.hpp"
#include "gfx/camera.hpp"

namespace SFG
{
	struct window_event;

	class world
	{
	public:
		enum flags
		{
			world_flags_is_init	   = 1 << 0,
			world_flags_is_playing = 1 << 1,
		};

	public:
		world();

		void init();
		void uninit();
		void tick(float dt);

		entity_id create_entity(const char* name = "entity");
		void	  destroy_entity(entity_id id);

#ifdef SFG_TOOLMODE
		void save(const char* path);
		void load(const char* path);
#endif
		bool on_window_event(const window_event& ev);

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

		inline world_resources& get_resources()
		{
			return _resources;
		}

		inline camera& get_camera()
		{
			return _camera;
		}

	private:
		world_resources					 _resources = {};
		text_allocator<MAX_ENTITIES * 5> _txt_allocator;
		bitmask<uint8>					 _flags = 0;

		pool_allocator<entity_meta, entity_id, MAX_ENTITIES>  _entity_metas		= {};
		pool_allocator<entity_pos, entity_id, MAX_ENTITIES>	  _entity_positions = {};
		pool_allocator<entity_rot, entity_id, MAX_ENTITIES>	  _entity_rotations = {};
		pool_allocator<entity_scale, entity_id, MAX_ENTITIES> _entity_scales	= {};
		entity_id											  _entity_count		= 0;
		vector<entity_id>									  _entity_free_list = {};
		camera												  _camera			= {};
	};
}
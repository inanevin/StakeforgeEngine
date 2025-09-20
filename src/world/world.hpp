// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/bitmask.hpp"
#include "memory/pool_allocator.hpp"
#include "common_world.hpp"
#include "data/vector.hpp"
#include "memory/text_allocator.hpp"
#include "world/world_resources.hpp"
#include "gfx/camera.hpp"

#include "entity_manager.hpp"

namespace SFG
{
	struct window_event;
	struct vector2ui16;

	class world_renderer;

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
		~world();

		void init();
		void uninit();
		void tick(const vector2ui16& res, float dt);

#ifdef SFG_TOOLMODE
		void save(const char* path);
		void load(const char* path);
#endif
		bool on_window_event(const window_event& ev);

		inline entity_manager& get_entity_manager()
		{
			return _entity_manager;
		}

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

		inline text_allocator<MAX_ENTITIES * 5>& get_text_allocator()
		{
			return _txt_allocator;
		}

		inline world_renderer* get_renderer() const
		{
			return _world_renderer;
		}

		inline void set_world_renderer(world_renderer* wr)
		{
			_world_renderer = wr;
		}

	private:
		world_renderer*					 _world_renderer = nullptr;
		world_resources					 _resources		 = {};
		text_allocator<MAX_ENTITIES * 5> _txt_allocator;
		bitmask<uint8>					 _flags = 0;
		entity_manager					 _entity_manager;
		camera							 _camera = {};
	};
}
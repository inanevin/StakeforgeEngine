// Copyright (c) 2025 Inan Evin

#include "world.hpp"
#include "io/log.hpp"
#include "io/file_system.hpp"
#include "project/engine_data.hpp"
#include "app/debug_console.hpp"

#ifdef SFG_TOOLMODE
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{

	world::world()
	{
		_entity_free_list.reserve(MAX_ENTITIES);
	}

	void world::init()
	{
		SFG_PROG("initializing world.");

		_entity_count = 0;
		_entity_free_list.resize(0);

		_flags.set(world_flags_is_init);
		debug_console::get()->register_console_function<int>("world_set_play", [this](int b) { _flags.set(world_flags_is_playing, b != 0); });

		_resources.init();
	}

	void world::uninit()
	{
		_resources.uninit();

		_txt_allocator.reset();

		SFG_PROG("uninitializing world.");
		_flags.remove(world_flags_is_init);
		debug_console::get()->unregister_console_function("world_set_play");

		_entity_metas.verify_uninit();
		_entity_positions.verify_uninit();
		_entity_rotations.verify_uninit();
		_entity_scales.verify_uninit();
	}

	void world::tick(float dt)
	{
	}

	entity_id world::create_entity(const char* name)
	{
		entity_id id = 0;

		if (_entity_free_list.empty())
		{
			id = _entity_count;
			_entity_count++;
			SFG_ASSERT(_entity_count < MAX_ENTITIES);
		}
		else
		{
			id = _entity_free_list.back();
			_entity_free_list.pop_back();
		}

		entity_meta& meta = _entity_metas.get(id);
		meta			  = {
						 .name	= _txt_allocator.allocate(name),
						 .flags = 0,
		 };

		return id;
	}

	void world::destroy_entity(entity_id id)
	{
		_entity_free_list.push_back(id);

		entity_meta&  meta	= _entity_metas.get(id);
		entity_pos&	  pos	= _entity_positions.get(id);
		entity_rot&	  rot	= _entity_rotations.get(id);
		entity_scale& scale = _entity_scales.get(id);

		_txt_allocator.deallocate((char*)meta.name);

		meta  = {};
		pos	  = {};
		rot	  = {};
		scale = {};
	}

#ifdef SFG_TOOLMODE

	void world::save(const char* path)
	{
		json j;
		j["dummy"] = 2;

		std::ofstream file(path);

		if (file.is_open())
		{
			file << j.dump(4);
			file.close();
		}

		SFG_PROG("saved world: {0}", path);
	}

	void world::load(const char* path)
	{

		if (!file_system::exists(path))
		{
			SFG_ERR("can't load world as path don't exist! {0}", path);
			return;
		}

		std::ifstream f(path);
		json		  data	= json::parse(f);
		int			  dummy = data["dummy"];
		SFG_PROG("loaded world {0}", path);
	}
#endif

	bool world::on_window_event(const window_event& ev)
	{
		return false;
	}
}
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
		: _entity_manager(*this){

		  };

	world::~world()
	{
	}

	void world::init()
	{
		SFG_PROG("initializing world.");
		_flags.set(world_flags_is_init);
		debug_console::get()->register_console_function<int>("world_set_play", [this](int b) { _flags.set(world_flags_is_playing, b != 0); });
		_resources.init(this);
		_entity_manager.init();
	}

	void world::uninit()
	{
		_entity_manager.uninit();
		_resources.uninit();
		_txt_allocator.reset();

		SFG_PROG("uninitializing world.");
		_flags.remove(world_flags_is_init);
		debug_console::get()->unregister_console_function("world_set_play");
	}

	void world::tick(const vector2ui16& res, float dt)
	{
		// do your stuff.
		auto& entities = _entity_manager.get_entities();

		for (pool_handle<world_id> handle : entities.handles())
		{
			const vector3& p = _entity_manager.get_entity_position_abs(handle);
			const quat&	   r = _entity_manager.get_entity_rotation_abs(handle);
			const vector3& s = _entity_manager.get_entity_scale_abs(handle);
			_entity_manager.set_entity_matrix(handle, matrix4x3::transform(p, r, s));
		}
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
		if (_flags.is_set(world_flags_is_init))
			uninit();

		if (strlen(path) == 0)
		{
			init();
			return;
		}

		if (!file_system::exists(path))
		{
			SFG_ERR("can't load world as path don't exist! {0}", path);
			return;
		}

		std::ifstream f(path);
		json		  data	= json::parse(f);
		int			  dummy = data["dummy"];
		SFG_PROG("loaded world {0}", path);

		init();
	}
#endif

	bool world::on_window_event(const window_event& ev)
	{
		if (!_flags.is_set(world_flags_is_init))
			return false;
		return false;
	}
}
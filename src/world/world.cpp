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

/* DEBUG */
#include <algorithm>
#include <execution>
#include "gfx/renderer.hpp"

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

	static vector<const char*> debug_textures = {
		"assets/boombox/boombox_base.stkfrg",
		"assets/boombox/boombox_emissive.stkfrg",
		"assets/boombox/boombox_normal.stkfrg",
		"assets/boombox/boombox_orm.stkfrg",
	};

	static vector<const char*> debug_mats = {
		"assets/boombox/boombox_material.stkfrg",
	};

	static vector<const char*> debug_shaders = {
		"assets/shaders/object/default_lit.stkfrg",
		"assets/shaders/object/default_lit_skinned.stkfrg",
	};

	static vector<const char*> debug_models = {
		"assets/boombox/boombox.gltf",
	};

	static vector<pool_handle<resource_id>> loaded_debug_textures = {};
	static vector<pool_handle<resource_id>> loaded_debug_mats	  = {};
	static vector<pool_handle<resource_id>> loaded_debug_shaders  = {};
	static vector<pool_handle<resource_id>> loaded_debug_models	  = {};

	void world::load_debug()
	{
		vector<std::function<void()>> tasks;

		/* Debug */
		for (const char* t : debug_textures)
		{
			const pool_handle<resource_id> handle = _resources.create_texture(TO_SIDC(t));
			loaded_debug_textures.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_texture(handle).create_from_file(abs.c_str()); });
		}

		for (const char* t : debug_shaders)
		{
			const pool_handle<resource_id> handle = _resources.create_shader(TO_SIDC(t));
			loaded_debug_shaders.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_shader(handle).create_from_file_vertex_pixel(abs.c_str(), false, renderer::get_bind_layout_global()); });
		}

		std::for_each(std::execution::par, tasks.begin(), tasks.end(), [](std::function<void()>& task) { task(); });

		for (const char* t : debug_mats)
			loaded_debug_mats.push_back(_resources.load_material(t));

		for (const char* t : debug_models)
			loaded_debug_models.push_back(_resources.load_model(t));
	}

	void world::uninit()
	{
		for (pool_handle<resource_id> handle : loaded_debug_textures)
		{
			_resources.get_texture(handle).destroy();
			_resources.destroy_texture(handle);
		}

		for (pool_handle<resource_id> handle : loaded_debug_mats)
		{
			_resources.get_material(handle).destroy();
			_resources.destroy_material(handle);
		}

		for (pool_handle<resource_id> handle : loaded_debug_shaders)
		{
			_resources.get_shader(handle).destroy();
			_resources.destroy_shader(handle);
		}

		for (pool_handle<resource_id> handle : loaded_debug_models)
		{
			_resources.destroy_model(handle);
		}

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
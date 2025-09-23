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
#include "resources/texture.hpp"
#include "resources/shader.hpp"
#include "resources/material.hpp"
#include "resources/model.hpp"
#include "platform/time.hpp"
#include "gfx/world/world_renderer.hpp"

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
		"assets/cesium_man/cesium_man_base.stkfrg",
	};

	static vector<const char*> debug_mats = {
		"assets/boombox/boombox_material.stkfrg",
		"assets/cesium_man/cesium_man_material.stkfrg",
	};

	static vector<const char*> debug_shaders = {
		"assets/shaders/object/default_lit.stkfrg",
		"assets/shaders/object/default_lit_skinned.stkfrg",
	};

	static vector<const char*> debug_models = {
		"assets/boombox/boombox.gltf",
		"assets/cesium_man/CesiumMan.gltf",
	};

	static vector<resource_handle> loaded_debug_textures	  = {};
	static vector<resource_handle> loaded_debug_mats		  = {};
	static vector<resource_handle> loaded_debug_shaders		  = {};
	static vector<model_loaded>	   loaded_debug_models_loaded = {};
	static vector<resource_handle> loaded_debug_models		  = {};

	void world::load_debug()
	{
		vector<std::function<void()>> tasks;

		const int64 mr_begin = time::get_cpu_microseconds();

		/* Debug */
		for (const char* t : debug_textures)
		{
			const resource_handle handle = _resources.create_resource<texture>(TO_SIDC(t));
			loaded_debug_textures.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_resource<texture>(handle).create_from_file(abs.c_str()); });
		}

		for (const char* t : debug_shaders)
		{
			const resource_handle handle = _resources.create_resource<shader>(TO_SIDC(t));
			loaded_debug_shaders.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_resource<shader>(handle).create_from_file_vertex_pixel(abs.c_str(), false, renderer::get_bind_layout_global()); });
		}

		std::for_each(std::execution::par, tasks.begin(), tasks.end(), [](std::function<void()>& task) { task(); });

		tasks.clear();

		for (const char* t : debug_mats)
		{
			const resource_handle handle = _resources.create_resource<material>(TO_SIDC(t));
			loaded_debug_mats.push_back(handle);
			const string abs = engine_data::get().get_working_dir() + t;
			tasks.push_back([handle, abs, this]() { _resources.get_resource<material>(handle).create_from_file(abs.c_str(), _resources); });
		}

		loaded_debug_models_loaded.resize(debug_models.size());
		int i = 0;
		for (const char* t : debug_models)
		{
			const resource_handle handle = _resources.create_resource<model>(TO_SIDC(t));
			loaded_debug_models.push_back(handle);

			const string relative = t;
			const string abs	  = engine_data::get().get_working_dir() + relative;
			const int	 index	  = i;
			tasks.push_back([handle, abs, relative, this, index]() { loaded_debug_models_loaded[index].create_from_file(abs.c_str(), relative.c_str()); });
			i++;
		}

		std::for_each(std::execution::par, tasks.begin(), tasks.end(), [](std::function<void()>& task) { task(); });

		i = 0;
		for (resource_handle handle : loaded_debug_models)
		{
			model& mdl = _resources.get_resource<model>(handle);
			mdl.create_from_loaded(loaded_debug_models_loaded[i], _resources.get_aux(), _resources);
			i++;
		}

		for (resource_handle h : loaded_debug_textures)
		{
			_world_renderer->get_resource_uploads().add_pending_texture(&_resources.get_resource<texture>(h));
		}

		for (resource_handle h : loaded_debug_models)
		{
			_world_renderer->get_resource_uploads().add_pending_model(&_resources.get_resource<model>(h));
		}

		for (resource_handle h : loaded_debug_mats)
		{
			_world_renderer->get_resource_uploads().add_pending_material(&_resources.get_resource<material>(h));
		}

		const int64 mr_diff = time::get_cpu_microseconds() - mr_begin;
		SFG_INFO("Resources took: {0} ms", mr_diff / 1000);
	}

	void world::uninit()
	{
		for (resource_handle handle : loaded_debug_textures)
		{
			_resources.get_resource<texture>(handle).destroy();
			_resources.destroy_resource<texture>(handle);
		}

		for (resource_handle handle : loaded_debug_mats)
		{
			_resources.get_resource<material>(handle).destroy();
			_resources.destroy_resource<material>(handle);
		}

		for (resource_handle handle : loaded_debug_shaders)
		{
			_resources.get_resource<shader>(handle).destroy();
			_resources.destroy_resource<shader>(handle);
		}

		for (resource_handle handle : loaded_debug_models)
		{
			_resources.get_resource<model>(handle).destroy(_resources, _resources.get_aux());

			_resources.destroy_resource<model>(handle);
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

		for (resource_handle handle : entities)
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
// Copyright (c) 2025 Inan Evin

#include "world_resources.hpp"
#include "app/debug_console.hpp"
#include "project/engine_data.hpp"
#include "world.hpp"
#include "world_renderer.hpp"

namespace SFG
{
	void world_resources::init(world* w)
	{
		_world = w;
		debug_console::get()->register_console_function<const char*>("world_load_texture", std::bind(&world_resources::load_texture, this, std::placeholders::_1));
	}

	void world_resources::uninit()
	{
		_world = nullptr;
		debug_console::get()->unregister_console_function("world_load_texture");
		_textures.verify_uninit();
		_models.verify_uninit();
		_anims.verify_uninit();
		_skins.verify_uninit();
	}

#ifdef SFG_TOOLMODE

	pool_handle<resource_id> world_resources::load_texture(const char* relative_path)
	{
		const string_id sid = TO_SIDC(relative_path);
		auto			it	= _texture_hashes.find(sid);
		if (it != _texture_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + "/" + relative_path;

		const pool_handle<resource_id> txt = create_texture(sid);
		texture&					   res = get_texture(txt);
		res.create_from_file(abs_path.c_str());

		_world->get_world_renderer()->add_pending_texture(&res);

		return txt;
	}

	pool_handle<resource_id> world_resources::load_model(const char* path)
	{
		const string_id sid = TO_SIDC(path);
		auto			it	= _model_hashes.find(sid);
		if (it != _model_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + "/" + path;

		const pool_handle<resource_id> handle = create_model(sid);
		model&						   mdl	  = get_model(handle);
		mdl.create_from_file(abs_path.c_str(), path, *this);
		_world->get_world_renderer()->add_pending_model(&mdl);

		return handle;
	}

	pool_handle<resource_id> world_resources::load_shader(const char* path)
	{
		const string_id sid = TO_SIDC(path);
		auto			it	= _shader_hashes.find(sid);
		if (it != _shader_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + "/" + path;

		const pool_handle<resource_id> handle = create_shader(sid);
		shader&						   res	  = get_shader(handle);
		res.create_from_file_vertex_pixel(abs_path.c_str(), true, 0);
		return handle;
	}

	pool_handle<resource_id> world_resources::load_material_pbr_default(const char* path)
	{
		const string_id sid = TO_SIDC(path);
		auto			it	= _material_pbr_default_hashes.find(sid);
		if (it != _material_pbr_default_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + "/" + path;

		const pool_handle<resource_id> handle = create_material_pbr_default(sid);
		material_pbr_default&		   mat	  = get_material_pbr_default(handle);
		mat.create_from_file(abs_path.c_str(), *this);
		return handle;
	}

#endif
}
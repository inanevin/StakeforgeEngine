// Copyright (c) 2025 Inan Evin

#include "world_resources.hpp"
#include "app/debug_console.hpp"
#include "project/engine_data.hpp"

namespace SFG
{

	void world_resources::init()
	{
		debug_console::get()->register_console_function<const char*>("world_load_texture", std::bind(&world_resources::load_texture, this, std::placeholders::_1));
	}

	void world_resources::uninit()
	{
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

	pool_handle<resource_id> world_resources::create_shader(string_id hash)
	{
		const pool_handle<resource_id> handle = _shaders.add();
		_shader_hashes[hash]				  = handle;
		return handle;
	}

	void world_resources::destroy_shader(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_shaders.is_valid(id));
		_shaders.remove(id);
	}

	shader& world_resources::get_shader(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_shaders.is_valid(id));
		return _shaders.get(id);
	}

	pool_handle<resource_id> world_resources::create_texture(string_id hash)
	{
		const pool_handle<resource_id> id = _textures.add();
		_texture_hashes[hash]			  = id;
		return id;
	}

	void world_resources::destroy_texture(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_textures.is_valid(id));
		_textures.remove(id);
	}

	texture& world_resources::get_texture(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_textures.is_valid(id));
		return _textures.get(id);
	}

	texture& world_resources::get_texture_by_hash(string_id hash)
	{
		const pool_handle<resource_id> res_id = _texture_hashes.at(hash);
		return _textures.get(res_id);
	}

	pool_handle<resource_id> world_resources::create_material_pbr_default(string_id hash)
	{
		const pool_handle<resource_id> handle = _materials_pbr_default.add();
		_material_pbr_default_hashes[hash]	  = handle;
		return handle;
	}

	void world_resources::destroy_material_pbr_default(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_materials_pbr_default.is_valid(id));
		_materials_pbr_default.remove(id);
	}

	material_pbr_default& world_resources::get_material_pbr_default(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_materials_pbr_default.is_valid(id));
		return _materials_pbr_default.get(id);
	}

	pool_handle<resource_id> world_resources::create_model(string_id hash)
	{
		const pool_handle<resource_id> id = _models.add();
		_model_hashes[hash]				  = id;
		return id;
	}

	void world_resources::destroy_model(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_models.is_valid(id));
		_models.remove(id);
	}

	model& world_resources::get_model(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_models.is_valid(id));
		return _models.get(id);
	}

	pool_handle<resource_id> world_resources::create_animation(string_id hash)
	{
		const pool_handle<resource_id> id = _anims.add();
		_anim_hashes[hash]				  = id;
		return id;
	}

	void world_resources::destroy_animation(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_anims.is_valid(id));
		_anims.remove(id);
	}

	animation& world_resources::get_animation(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_anims.is_valid(id));
		return _anims.get(id);
	}

	pool_handle<resource_id> world_resources::create_skin(string_id hash)
	{
		const pool_handle<resource_id> id = _skins.add();
		_skin_hashes[hash]				  = id;
		return id;
	}

	void world_resources::destroy_skin(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_skins.is_valid(id));
		_skins.remove(id);
	}

	skin& world_resources::get_skin(pool_handle<resource_id> id)
	{
		SFG_ASSERT(_skins.is_valid(id));
		return _skins.get(id);
	}

}
// Copyright (c) 2025 Inan Evin

#include "world_resources.hpp"
#include "gfx/world/world_renderer.hpp"
#include "app/debug_console.hpp"
#include "project/engine_data.hpp"
#include "world/world.hpp"
#include "gfx/renderer.hpp"
#include "world/common_world.hpp"

#include "resources/texture.hpp"
#include "resources/texture_sampler.hpp"
#include "resources/model.hpp"
#include "resources/shader.hpp"
#include "resources/skin.hpp"
#include "resources/animation.hpp"
#include "resources/material.hpp"

namespace SFG
{

	world_resources::world_resources()
	{
		_aux_memory.init(1024 * 1024 * 4);
		_storages.resize(resource_types::resource_type_allowed_max);

		init_storage<texture>(MAX_WORLD_TEXTURES);
		init_storage<texture_sampler>(MAX_WORLD_TEXTURES);
		init_storage<model>(MAX_WORLD_MODELS);
		init_storage<animation>(MAX_WORLD_ANIMS);
		init_storage<skin>(MAX_WORLD_SKINS);
		init_storage<shader>(MAX_WORLD_SHADERS);
		init_storage<material>(MAX_WORLD_MATERIALS);
		init_storage<mesh>(MAX_WORLD_MESHES);
	}

	world_resources::~world_resources()
	{
		_aux_memory.uninit();
		for (resource_storage& stg : _storages)
		{
			if (stg.storage.get_raw())
				stg.storage.uninit();
		}
	}

	void world_resources::init(world* w)
	{
		_world = w;
		debug_console::get()->register_console_function<const char*>("world_load_texture", std::bind(&world_resources::load_texture, this, std::placeholders::_1));
	}

	void world_resources::uninit()
	{
		_aux_memory.reset();
		for (resource_storage& stg : _storages)
			stg.storage.reset();

		_world = nullptr;
		debug_console::get()->unregister_console_function("world_load_texture");
	}

#ifdef SFG_TOOLMODE

	resource_handle world_resources::load_texture(const char* relative_path)
	{
		resource_storage& stg = _storages[texture::TYPE_INDEX];

		const string_id sid = TO_SIDC(relative_path);
		auto			it	= stg.by_hashes.find(sid);
		if (it != stg.by_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + relative_path;

		const resource_handle txt = create_resource<texture>(sid);
		texture&			  res = get_resource<texture>(txt);

		if (res.create_from_file(abs_path.c_str()))
		{
			SFG_INFO("Loaded texture: {0}", abs_path);
			_world->get_renderer()->get_resource_uploads().add_pending_texture(&res);
		}
		else
		{
			SFG_ERR("Failed loading texture: {0}", abs_path);
			destroy_resource<texture>(txt);
		}

		return txt;
	}

	resource_handle world_resources::load_model(const char* path)
	{
		resource_storage& stg = _storages[model::TYPE_INDEX];

		const string_id sid = TO_SIDC(path);
		auto			it	= stg.by_hashes.find(sid);
		if (it != stg.by_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + path;

		const resource_handle handle = create_resource<model>(sid);
		model&				  mdl	 = get_resource<model>(handle);

		model_loaded loaded = {};
		if (loaded.create_from_file(abs_path.c_str(), path))
		{
			mdl.create_from_loaded(loaded, _aux_memory, *this);
			SFG_INFO("Loaded model: {0}", abs_path);

			const chunk_handle32 created_meshes = mdl.get_created_meshes();
			const uint16		 meshes_count	= mdl.get_mesh_count();

			chunk_allocator32& aux	   = get_aux();
			resource_handle*   handles = meshes_count == 0 ? nullptr : aux.get<resource_handle>(created_meshes);

			for (uint16 i = 0; i < meshes_count; i++)
			{
				resource_handle& handle = handles[i];
				mesh&			 m		= get_resource<mesh>(handle);
				_world->get_renderer()->get_resource_uploads().add_pending_mesh(&m);
			}
		}
		else
		{
			SFG_ERR("Failed loading model: {0}", abs_path);
			destroy_resource<model>(handle);
		}

		return handle;
	}

	resource_handle world_resources::load_shader(const char* path)
	{
		resource_storage& stg = _storages[shader::TYPE_INDEX];

		const string_id sid = TO_SIDC(path);
		auto			it	= stg.by_hashes.find(sid);
		if (it != stg.by_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + path;

		const resource_handle handle = create_resource<shader>(sid);
		shader&				  res	 = get_resource<shader>(handle);

		if (res.create_from_file_vertex_pixel(abs_path.c_str(), false, renderer::get_bind_layout_global()))
		{
			SFG_INFO("Loaded shader: {0}", abs_path);
		}
		else
		{
			SFG_ERR("Failed loading shader: {0}", abs_path);
			destroy_resource<shader>(handle);
		}

		return handle;
	}

	resource_handle world_resources::load_material(const char* path)
	{
		resource_storage& stg = _storages[material::TYPE_INDEX];

		const string_id sid = TO_SIDC(path);
		auto			it	= stg.by_hashes.find(sid);
		if (it != stg.by_hashes.end())
			return it->second;
		const string abs_path = engine_data::get().get_working_dir() + path;

		const resource_handle handle = create_resource<material>(sid);
		material&			  mat	 = get_resource<material>(handle);

		if (mat.create_from_file(abs_path.c_str(), *this))
		{
			SFG_INFO("Loaded material: {0}", abs_path);
		}
		else
		{
			SFG_ERR("Failed loading material: {0}", abs_path);
			destroy_resource<material>(handle);
		}
		return handle;
	}

#endif
}
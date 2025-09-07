// Copyright (c) 2025 Inan Evin

#pragma once

#include "resources/texture.hpp"
#include "resources/model.hpp"
#include "resources/shader.hpp"
#include "resources/skin.hpp"
#include "resources/animation.hpp"
#include "material_pbr_default.hpp"
#include "memory/pool_allocator.hpp"
#include "game/common_world.hpp"
#include "data/hash_map.hpp"
#include "data/string_id.hpp"

namespace SFG
{

#define MAX_WORLD_TEXTURES	  32
#define MAX_WORLD_MODELS	  32
#define MAX_WORLD_ANIMS		  32
#define MAX_WORLD_SKINS		  12
#define MAX_WORLD_MAT_PBR_DEF 20
#define MAX_WORLD_SHADERS	  30

	class world_resources
	{
	public:
		void init();
		void uninit();

#ifdef SFG_TOOLMODE
		pool_handle<resource_id> load_texture(const char* path);
		pool_handle<resource_id> load_model(const char* path);
		pool_handle<resource_id> load_shader(const char* path);
		pool_handle<resource_id> load_material_pbr_default(const char* path);
#endif

		pool_handle<resource_id> create_shader(string_id hash);
		void					 destroy_shader(pool_handle<resource_id> id);
		shader&					 get_shader(pool_handle<resource_id> id);

		pool_handle<resource_id> create_texture(string_id hash);
		void					 destroy_texture(pool_handle<resource_id> id);
		texture&				 get_texture(pool_handle<resource_id> id);
		texture&				 get_texture_by_hash(string_id hash);

		pool_handle<resource_id> create_material_pbr_default(string_id hash);
		void					 destroy_material_pbr_default(pool_handle<resource_id> id);
		material_pbr_default&	 get_material_pbr_default(pool_handle<resource_id> id);

		pool_handle<resource_id> create_model(string_id hash);
		void					 destroy_model(pool_handle<resource_id> id);
		model&					 get_model(pool_handle<resource_id> id);

		pool_handle<resource_id> create_animation(string_id hash);
		void					 destroy_animation(pool_handle<resource_id> id);
		animation&				 get_animation(pool_handle<resource_id> id);

		pool_handle<resource_id> create_skin(string_id hash);
		void					 destroy_skin(pool_handle<resource_id> id);
		skin&					 get_skin(pool_handle<resource_id> id);

		inline pool_allocator_gen<texture, resource_id, MAX_WORLD_TEXTURES>& get_textures()
		{
			return _textures;
		}

		inline pool_allocator_gen<model, resource_id, MAX_WORLD_MODELS>& get_models()
		{
			return _models;
		}

		inline pool_allocator_gen<material_pbr_default, resource_id, MAX_WORLD_MAT_PBR_DEF>& get_materials_pbr_default()
		{
			return _materials_pbr_default;
		}

	private:
		hash_map<string_id, pool_handle<resource_id>> _texture_hashes;
		hash_map<string_id, pool_handle<resource_id>> _anim_hashes;
		hash_map<string_id, pool_handle<resource_id>> _skin_hashes;
		hash_map<string_id, pool_handle<resource_id>> _model_hashes;
		hash_map<string_id, pool_handle<resource_id>> _material_pbr_default_hashes;
		hash_map<string_id, pool_handle<resource_id>> _shader_hashes;

		pool_allocator_gen<texture, resource_id, MAX_WORLD_TEXTURES>				 _textures;
		pool_allocator_gen<model, resource_id, MAX_WORLD_MODELS>					 _models;
		pool_allocator_gen<animation, resource_id, MAX_WORLD_ANIMS>					 _anims;
		pool_allocator_gen<skin, resource_id, MAX_WORLD_SKINS>						 _skins;
		pool_allocator_gen<shader, resource_id, MAX_WORLD_SHADERS>					 _shaders;
		pool_allocator_gen<material_pbr_default, resource_id, MAX_WORLD_MAT_PBR_DEF> _materials_pbr_default;
	};
}

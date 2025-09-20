// Copyright (c) 2025 Inan Evin

#pragma once

#include "resources/texture.hpp"
#include "resources/model.hpp"
#include "resources/shader.hpp"
#include "resources/skin.hpp"
#include "resources/animation.hpp"
#include "resources/material.hpp"
#include "memory/pool_allocator.hpp"
#include "world/common_world.hpp"
#include "data/hash_map.hpp"
#include "data/string_id.hpp"

namespace SFG
{

	// clang-format off

#define DEFINE_RES_FUNCS(RESOURCETYPE, HASHES, STORAGE) \
	pool_handle<resource_id> create_##RESOURCETYPE(string_id hash) { \
		pool_handle<resource_id> handle = ##STORAGE.add(); \
		##HASHES[hash] = handle; \
		return handle; \
	} \
	void destroy_##RESOURCETYPE(pool_handle<resource_id> handle) { \
		##STORAGE.remove(handle); \
	} \
	##RESOURCETYPE& get_##RESOURCETYPE(pool_handle<resource_id> handle) { \
		auto& res = ##STORAGE.get(handle); \
		return res; \
	} \
	##RESOURCETYPE& get_##RESOURCETYPE##_by_hash(string_id hash) { \
		pool_handle<resource_id> handle = ##HASHES.at(hash); \
		auto& res = ##STORAGE.get(handle); \
		return res; \
	} \
	pool_handle<resource_id> get_##RESOURCETYPE##_handle_by_hash(string_id hash) { \
		pool_handle<resource_id> handle = ##HASHES.at(hash); \
		return handle; \
	} \
	inline auto& get_##RESOURCETYPE##s() {\
		auto& str = ##STORAGE; \
		return str;\
	}

	// clang-format on

	class world;

	class world_resources
	{
	public:
		void init(world* w);
		void uninit();

#ifdef SFG_TOOLMODE
		pool_handle<resource_id> load_texture(const char* path);
		pool_handle<resource_id> load_model(const char* path);
		pool_handle<resource_id> load_shader(const char* path);
		pool_handle<resource_id> load_material(const char* path);
#endif

		DEFINE_RES_FUNCS(texture, _texture_hashes, _textures);
		DEFINE_RES_FUNCS(shader, _shader_hashes, _shaders);
		DEFINE_RES_FUNCS(model, _model_hashes, _models);
		DEFINE_RES_FUNCS(material, _material_hashes, _materials);
		DEFINE_RES_FUNCS(animation, _anim_hashes, _anims);
		DEFINE_RES_FUNCS(skin, _skin_hashes, _skins);

	private:
		world*										  _world = nullptr;
		hash_map<string_id, pool_handle<resource_id>> _texture_hashes;
		hash_map<string_id, pool_handle<resource_id>> _anim_hashes;
		hash_map<string_id, pool_handle<resource_id>> _skin_hashes;
		hash_map<string_id, pool_handle<resource_id>> _model_hashes;
		hash_map<string_id, pool_handle<resource_id>> _material_hashes;
		hash_map<string_id, pool_handle<resource_id>> _shader_hashes;

		pool_allocator_gen<texture, resource_id, MAX_WORLD_TEXTURES>	 _textures;
		pool_allocator_gen<model, resource_id, MAX_WORLD_MODELS>		 _models;
		pool_allocator_gen<animation, resource_id, MAX_WORLD_ANIMS>		 _anims;
		pool_allocator_gen<skin, resource_id, MAX_WORLD_SKINS>			 _skins;
		pool_allocator_gen<shader, resource_id, MAX_WORLD_SHADERS>		 _shaders;
		pool_allocator_gen<material, resource_id, MAX_WORLD_MAT_PBR_DEF> _materials;
	};
}

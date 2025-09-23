// Copyright (c) 2025 Inan Evin

#pragma once

#include "memory/pool_allocator16.hpp"
#include "memory/chunk_allocator.hpp"
#include "data/hash_map.hpp"
#include "data/string_id.hpp"
#include "data/static_vector.hpp"
#include "common_world.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	class world;

	struct resource_storage
	{
		pool_allocator16					 storage;
		hash_map<string_id, resource_handle> by_hashes;
	};

	class world_resources
	{
	public:
		world_resources();
		~world_resources();

		void init(world* w);
		void uninit();

		template <typename T> void destroy_resource(resource_handle handle)
		{
			SFG_ASSERT(T::TYPE_INDEX < _storages.size());
			resource_storage& stg = _storages[T::TYPE_INDEX];
			stg.storage.free<T>(handle);
		}

		template <typename T> resource_handle get_resource_handle_by_hash(string_id hash) const
		{
			SFG_ASSERT(T::TYPE_INDEX < _storages.size());
			resource_storage& stg = _storages[T::TYPE_INDEX];
			return stg.by_hashes.at(hash);
		}

		template <typename T> resource_handle create_resource(string_id hash)
		{
			SFG_ASSERT(T::TYPE_INDEX < _storages.size());
			resource_storage&	  stg	 = _storages[T::TYPE_INDEX];
			const resource_handle handle = stg.storage.allocate<T>();
			stg.by_hashes[hash]			 = handle;
			return handle;
		}

		template <typename T> T& get_resource(resource_handle handle) const
		{
			SFG_ASSERT(T::TYPE_INDEX < _storages.size());
			return _storages[T::TYPE_INDEX].storage.get<T>(handle);
		}

		template <typename T> T& get_resource_by_hash(string_id hash) const
		{
			SFG_ASSERT(T::TYPE_INDEX < _storages.size());
			resource_storage&	  stg	 = _storages[T::TYPE_INDEX];
			const resource_handle handle = stg.by_hashes.at(hash);
			return stg.storage.get<T>(handle);
		}

#ifdef SFG_TOOLMODE
		resource_handle load_texture(const char* path);
		resource_handle load_model(const char* path);
		resource_handle load_shader(const char* path);
		resource_handle load_material(const char* path);
#endif

		inline static_vector<resource_storage, resource_type_allowed_max>& get_storages()
		{
			return _storages;
		}

		inline chunk_allocator32& get_aux()
		{
			return _aux_memory;
		}

	private:
		world* _world = nullptr;

		mutable static_vector<resource_storage, resource_type_allowed_max> _storages;
		chunk_allocator32												   _aux_memory;
	};
}

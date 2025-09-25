// Copyright (c) 2025 Inan Evin
#pragma once

#include "data/bitmask.hpp"
#include "math/aabb.hpp"
#include "memory/chunk_handle.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	class world_resources;
	class chunk_allocator32;
	struct model_raw;

	class model
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_model;

		enum flags
		{
			hw_exists	   = 1 << 0,
			pending_upload = 1 << 1,
		};

		~model();

		void create_from_raw(model_raw& raw, chunk_allocator32& alloc, world_resources& resources);
		void destroy(world_resources& resources, chunk_allocator32& alloc);

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

		inline const aabb& get_total_aabb() const
		{
			return _total_aabb;
		}

		inline uint8 get_material_count() const
		{
			return _material_count;
		}

		inline chunk_handle32 get_created_meshes() const
		{
			return _created_meshes;
		}

		inline chunk_handle32 get_nodes() const
		{
			return _nodes;
		}

		inline uint16 get_node_count() const
		{
			return _nodes_count;
		}

		inline uint16 get_mesh_count() const
		{
			return _meshes_count;
		}

	private:
#ifdef SFG_TOOLMODE
		friend struct model_raw;
#endif
		chunk_handle32 _nodes;
		chunk_handle32 _created_meshes;
		chunk_handle32 _created_skins;
		chunk_handle32 _created_anims;
		aabb		   _total_aabb;
		uint16		   _nodes_count	 = 0;
		uint16		   _meshes_count = 0;
		uint16		   _skins_count	 = 0;
		uint16		   _anims_count	 = 0;
		bitmask<uint8> _flags;
		uint8		   _material_count = 0;
	};

}
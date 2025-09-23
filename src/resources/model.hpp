// Copyright (c) 2025 Inan Evin
#pragma once

#include "data/vector.hpp"
#include "data/bitmask.hpp"
#include "animation.hpp"
#include "skin.hpp"
#include "mesh.hpp"
#include "model_node.hpp"
#include "math/aabb.hpp"
#include "memory/chunk_handle.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	class world_resources;
	class chunk_allocator32;

#ifdef SFG_TOOLMODE

	struct model_loaded
	{
		vector<model_node_loaded> loaded_nodes;
		vector<mesh_loaded>		  loaded_meshes;
		vector<skin_loaded>		  loaded_skins;
		vector<animation_loaded>  loaded_animations;
		aabb					  total_aabb;
		uint8					  material_count = 0;

		bool create_from_file(const char* file, const char* relative_path);
	};

#endif

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

#ifdef SFG_TOOLMODE
		void create_from_loaded(const model_loaded& loaded, chunk_allocator32& alloc, world_resources& resources);
#endif

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

		inline chunk_handle32 get_meshes() const
		{
			return _meshes;
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
		chunk_handle32 _nodes;
		chunk_handle32 _meshes;
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
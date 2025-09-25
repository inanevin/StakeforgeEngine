// Copyright (c) 2025 Inan Evin
#include "model.hpp"
#include "memory/chunk_allocator.hpp"
#include "mesh.hpp"
#include "model_node.hpp"
#include "skin.hpp"
#include "animation.hpp"
#include "model_raw.hpp"
#include "world/world_resources.hpp"

namespace SFG
{
	model::~model()
	{
		SFG_ASSERT(!_flags.is_set(model::flags::hw_exists));
	}

	void model::create_from_raw(model_raw& raw, chunk_allocator32& alloc, world_resources& resources)
	{
		SFG_ASSERT(!_flags.is_set(model::flags::hw_exists));

		_total_aabb		= raw.total_aabb;
		_material_count = raw.material_count;

		const uint16 node_count	 = static_cast<uint16>(raw.loaded_nodes.size());
		const uint16 mesh_count	 = static_cast<uint16>(raw.loaded_meshes.size());
		const uint16 skins_count = static_cast<uint16>(raw.loaded_skins.size());
		const uint16 anims_count = static_cast<uint16>(raw.loaded_animations.size());

		if (node_count != 0)
		{
			_nodes				  = alloc.allocate<model_node>(node_count);
			model_node* ptr_nodes = alloc.get<model_node>(_nodes);

			for (uint16 i = 0; i < node_count; i++)
			{
				const model_node_raw& loaded_node = raw.loaded_nodes[i];
				model_node&			  node		  = ptr_nodes[i];
				node.create_from_raw(loaded_node, alloc);
			}
		}

		if (mesh_count != 0)
		{
			_created_meshes				= alloc.allocate<resource_handle>(mesh_count);
			resource_handle* meshes_ptr = alloc.get<resource_handle>(_created_meshes);

			for (uint16 i = 0; i < mesh_count; i++)
			{
				const mesh_raw&		  loaded_mesh = raw.loaded_meshes[i];
				const resource_handle handle	  = resources.create_resource<mesh>(loaded_mesh.sid);
				meshes_ptr[i]					  = handle;

				mesh& m = resources.get_resource<mesh>(handle);
				m.create_from_raw(loaded_mesh, alloc);
			}
		}

		if (skins_count != 0)
		{
			_created_skins			   = alloc.allocate<resource_handle>(skins_count);
			resource_handle* skins_ptr = alloc.get<resource_handle>(_created_skins);

			for (uint16 i = 0; i < skins_count; i++)
			{
				const skin_raw&		  loaded_skin = raw.loaded_skins[i];
				const resource_handle handle	  = resources.create_resource<skin>(loaded_skin.sid);
				skins_ptr[i]					  = handle;
				skin& created					  = resources.get_resource<skin>(handle);
				created.create_from_raw(loaded_skin, alloc);
			}
		}

		if (anims_count != 0)
		{
			_created_anims			   = alloc.allocate<resource_handle>(anims_count);
			resource_handle* anims_ptr = alloc.get<resource_handle>(_created_anims);

			for (uint16 i = 0; i < anims_count; i++)
			{
				const animation_raw&  loaded_anim = raw.loaded_animations[i];
				const resource_handle handle	  = resources.create_resource<animation>(loaded_anim.sid);
				anims_ptr[i]					  = handle;
				animation& created				  = resources.get_resource<animation>(handle);
				created.create_from_raw(loaded_anim, alloc);
			}
		}

		_skins_count  = skins_count;
		_anims_count  = anims_count;
		_meshes_count = mesh_count;
		_nodes_count  = node_count;
		_flags.set(model::flags::pending_upload | model::flags::hw_exists);
	}

	void model::destroy(world_resources& resources, chunk_allocator32& alloc)
	{
		SFG_ASSERT(_flags.is_set(model::flags::hw_exists));

		if (_nodes.size != 0)
		{
			model_node* ptr_nodes = alloc.get<model_node>(_nodes);

			for (uint16 i = 0; i < _nodes_count; i++)
			{
				model_node& node = ptr_nodes[i];
				node.destroy(alloc);
			}

			alloc.free(_nodes);
		}

		if (_created_meshes.size != 0)
		{
			resource_handle* ptr = alloc.get<resource_handle>(_created_meshes);
			for (uint16 i = 0; i < _meshes_count; i++)
			{
				const resource_handle handle = ptr[i];
				mesh&				  m		 = resources.get_resource<mesh>(handle);
				m.destroy(alloc);
				resources.destroy_resource<mesh>(handle);
			}

			alloc.free(_created_meshes);
		}

		if (_created_skins.size != 0)
		{
			resource_handle* skins_ptr = alloc.get<resource_handle>(_created_skins);

			for (uint16 i = 0; i < _skins_count; i++)
			{
				const resource_handle handle = skins_ptr[i];
				skin&				  sk	 = resources.get_resource<skin>(handle);
				sk.destroy(alloc);
				resources.destroy_resource<skin>(handle);
			}

			alloc.free(_created_skins);
		}

		if (_created_anims.size != 0)
		{
			resource_handle* anims_ptr = alloc.get<resource_handle>(_created_anims);

			for (uint16 i = 0; i < _anims_count; i++)
			{
				const resource_handle handle = anims_ptr[i];
				animation&			  anim	 = resources.get_resource<animation>(handle);
				anim.destroy(alloc);
				resources.destroy_resource<animation>(handle);
			}

			alloc.free(_created_anims);
		}

		_created_meshes = {};
		_nodes			= {};
		_created_skins	= {};
		_created_anims	= {};
		_flags.remove(model::flags::hw_exists);
	}
}
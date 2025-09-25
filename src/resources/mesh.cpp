// Copyright (c) 2025 Inan Evin

#include "mesh.hpp"
#include "mesh_raw.hpp"
#include "primitive_raw.hpp"
#include "primitive.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{

	void mesh::create_from_raw(const mesh_raw& raw, chunk_allocator32& alloc)
	{
		if (!raw.name.empty())
		{
			_name = alloc.allocate<uint8>(raw.name.size());
			strcpy((char*)alloc.get(_name.head), raw.name.data());
			strcpy((char*)(alloc.get(_name.head + _name.size)), "\0");
		}

		_node_index				  = raw.node_index;
		_primitives_static_count  = static_cast<uint16>(raw.primitives_static.size());
		_primitives_skinned_count = static_cast<uint16>(raw.primitives_skinned.size());

		vector<uint16> materials;

		auto add_material = [&](uint16 m) {
			int32 index = vector_util::index_of(materials, m);
			if (index == -1)
				materials.push_back(m);
		};

		if (!raw.primitives_static.empty())
		{
			_primitives_static = alloc.allocate<primitive>(raw.primitives_static.size());

			primitive*	 ptr		 = alloc.get<primitive>(_primitives_static);
			const uint32 prims_count = static_cast<uint32>(raw.primitives_static.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_static_raw& prim_loaded = raw.primitives_static[i];
				primitive&					prim		= ptr[i];
				prim.indices_count						= static_cast<uint32>(prim_loaded.indices.size());
				add_material(prim.material_index);

				prim.indices  = alloc.allocate<primitive_index>(prim_loaded.indices.size());
				prim.vertices = alloc.allocate<vertex_static>(prim_loaded.vertices.size());
				SFG_MEMCPY(alloc.get(prim.indices.head), prim_loaded.indices.data(), sizeof(primitive_index) * prim_loaded.indices.size());
				SFG_MEMCPY(alloc.get(prim.vertices.head), prim_loaded.vertices.data(), sizeof(vertex_static) * prim_loaded.vertices.size());
			}
		}

		if (!raw.primitives_skinned.empty())
		{
			_primitives_skinned = alloc.allocate<primitive>(raw.primitives_skinned.size());

			primitive*	 ptr		 = alloc.get<primitive>(_primitives_skinned);
			const uint32 prims_count = static_cast<uint32>(raw.primitives_skinned.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_skinned_raw& prim_loaded = raw.primitives_skinned[i];
				primitive&					 prim		 = ptr[i];
				prim.indices_count						 = static_cast<uint32>(prim_loaded.indices.size());

				add_material(prim.material_index);
				prim.indices  = alloc.allocate<primitive_index>(prim_loaded.indices.size());
				prim.vertices = alloc.allocate<vertex_skinned>(prim_loaded.vertices.size());
				SFG_MEMCPY(alloc.get(prim.indices.head), prim_loaded.indices.data(), sizeof(primitive_index) * prim_loaded.indices.size());
				SFG_MEMCPY(alloc.get(prim.vertices.head), prim_loaded.vertices.data(), sizeof(vertex_skinned) * prim_loaded.vertices.size());
			}
		}

		_material_count	  = static_cast<uint16>(materials.size());
		_material_indices = alloc.allocate<uint16>(materials.size());
		SFG_MEMCPY(alloc.get(_material_indices.head), materials.data(), sizeof(uint16) * materials.size());

		// run through again to localize material indices.

		if (!raw.primitives_static.empty())
		{
			primitive*	 ptr		 = alloc.get<primitive>(_primitives_static);
			const uint32 prims_count = static_cast<uint32>(raw.primitives_static.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_static_raw& prim_loaded = raw.primitives_static[i];
				primitive&					prim		= ptr[i];
				prim.material_index						= static_cast<uint16>(vector_util::index_of(materials, prim.material_index));
			}
		}

		if (!raw.primitives_skinned.empty())
		{
			primitive*	 ptr		 = alloc.get<primitive>(_primitives_skinned);
			const uint32 prims_count = static_cast<uint32>(raw.primitives_skinned.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_skinned_raw& prim_loaded = raw.primitives_skinned[i];
				primitive&					 prim		 = ptr[i];
				prim.material_index						 = static_cast<uint16>(vector_util::index_of(materials, prim.material_index));
			}
		}
	}

	void mesh::destroy(chunk_allocator32& alloc)
	{
		if (_name.size != 0)
			alloc.free(_name);

		if (_primitives_static.size != 0)
		{
			primitive* ptr = alloc.get<primitive>(_primitives_static);

			const uint32 prim_count = _primitives_static_count;
			for (uint32 i = 0; i < prim_count; i++)
			{
				primitive& prim = ptr[i];
				alloc.free(prim.indices);
				alloc.free(prim.vertices);
			}

			alloc.free(_primitives_static);
		}

		if (_primitives_skinned.size != 0)
		{
			primitive*	 ptr		= alloc.get<primitive>(_primitives_skinned);
			const uint32 prim_count = _primitives_skinned_count;
			for (uint32 i = 0; i < prim_count; i++)
			{
				primitive& prim = ptr[i];
				alloc.free(prim.indices);
				alloc.free(prim.vertices);
			}

			alloc.free(_primitives_skinned);
		}

		alloc.free(_material_indices);

		_name					  = {};
		_primitives_static		  = {};
		_primitives_skinned		  = {};
		_material_indices		  = {};
		_primitives_static_count  = 0;
		_primitives_skinned_count = 0;
		_material_count			  = 0;
	}
}
// Copyright (c) 2025 Inan Evin

#include "mesh.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
	void mesh::create_from_loaded(const mesh_loaded& loaded, chunk_allocator32& alloc)
	{
		if (!loaded.name.empty())
		{
			name = alloc.allocate<uint8>(loaded.name.size());
			strcpy((char*)alloc.get(name.head), loaded.name.data());
		}
		node_index = loaded.node_index;
		if (!loaded.primitives_static.empty())
		{
			primitives_static = alloc.allocate<primitive>(loaded.primitives_static.size());

			primitive*	 ptr		 = alloc.get<primitive>(primitives_static);
			const uint32 prims_count = static_cast<uint32>(loaded.primitives_static.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_static_loaded& prim_loaded = loaded.primitives_static[i];
				primitive&					   prim		   = ptr[i];
				prim.material_index						   = prim_loaded.material_index;
				prim.indices							   = alloc.allocate<primitive_index>(prim_loaded.indices.size());
				prim.vertices							   = alloc.allocate<vertex_static>(prim_loaded.vertices.size());
				SFG_MEMCPY(alloc.get(prim.indices.head), prim_loaded.indices.data(), sizeof(primitive_index) * prim_loaded.indices.size());
				SFG_MEMCPY(alloc.get(prim.vertices.head), prim_loaded.vertices.data(), sizeof(vertex_static) * prim_loaded.vertices.size());
			}
		}

		if (!loaded.primitives_skinned.empty())
		{
			primitives_skinned = alloc.allocate<primitive>(loaded.primitives_skinned.size());

			primitive*	 ptr		 = alloc.get<primitive>(primitives_skinned);
			const uint32 prims_count = static_cast<uint32>(loaded.primitives_skinned.size());
			for (uint32 i = 0; i < prims_count; i++)
			{
				const primitive_skinned_loaded& prim_loaded = loaded.primitives_skinned[i];
				primitive&						prim		= ptr[i];

				prim.material_index = prim_loaded.material_index;
				prim.indices		= alloc.allocate<primitive_index>(prim_loaded.indices.size());
				prim.vertices		= alloc.allocate<vertex_skinned>(prim_loaded.vertices.size());
				SFG_MEMCPY(alloc.get(prim.indices.head), prim_loaded.indices.data(), sizeof(primitive_index) * prim_loaded.indices.size());
				SFG_MEMCPY(alloc.get(prim.vertices.head), prim_loaded.vertices.data(), sizeof(vertex_skinned) * prim_loaded.vertices.size());
			}
		}
	}

	void mesh::destroy(chunk_allocator32& alloc)
	{
		if (name.size != 0)
			alloc.free(name);

		if (primitives_static.size != 0)
		{
			primitive* ptr = alloc.get<primitive>(primitives_static);

			const uint32 prim_count = sizeof(primitive) / primitives_static.size;
			for (uint32 i = 0; i < prim_count; i++)
			{
				primitive& prim = ptr[i];
				alloc.free(prim.indices);
				alloc.free(prim.vertices);
			}

			alloc.free(primitives_static);
		}

		if (primitives_skinned.size != 0)
		{
			primitive*	 ptr		= alloc.get<primitive>(primitives_skinned);
			const uint32 prim_count = sizeof(primitive) / primitives_skinned.size;
			for (uint32 i = 0; i < prim_count; i++)
			{
				primitive& prim = ptr[i];
				alloc.free(prim.indices);
				alloc.free(prim.vertices);
			}

			alloc.free(primitives_skinned);
		}

		name			   = {};
		primitives_static  = {};
		primitives_skinned = {};
	}
}
// Copyright (c) 2025 Inan Evin

#include "model_node.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
	void model_node::create_from_loaded(const model_node_loaded& loaded, chunk_allocator32& alloc)
	{
		if (!loaded.name.empty())
		{
			name = alloc.allocate<uint8>(loaded.name.size());
			strcpy((char*)alloc.get(name.head), loaded.name.data());
		}

		parent_index = loaded.parent_index;
		mesh_index	 = loaded.mesh_index;
		local_matrix = loaded.local_matrix;
	}

	void model_node::destroy(chunk_allocator32& alloc)
	{
		if (name.size != 0)
			alloc.free(name);
	}
}
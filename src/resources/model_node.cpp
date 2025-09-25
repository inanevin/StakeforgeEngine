// Copyright (c) 2025 Inan Evin

#include "model_node.hpp"
#include "model_node_raw.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
	void model_node::destroy(chunk_allocator32& alloc)
	{
		if (_name.size != 0)
			alloc.free(_name);
	}

	void model_node::create_from_raw(const model_node_raw& raw, chunk_allocator32& alloc)
	{
		if (!raw.name.empty())
		{
			_name = alloc.allocate<uint8>(raw.name.size() + 1);
			strcpy((char*)alloc.get(_name.head), raw.name.data());
			strcpy((char*)(alloc.get(_name.head + _name.size)), "\0");
		}

		_parent_index = raw.parent_index;
		_mesh_index	  = raw.mesh_index;
		_local_matrix = raw.local_matrix;
	}

}
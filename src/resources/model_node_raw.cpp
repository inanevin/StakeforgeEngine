// Copyright (c) 2025 Inan Evin

#include "model_node_raw.hpp"
#include "model_node.hpp"
#include "memory/chunk_allocator.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	void model_node_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << parent_index;
		stream << mesh_index;
		stream << local_matrix;
	}

	void model_node_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> parent_index;
		stream >> mesh_index;
		stream >> local_matrix;
	}
}

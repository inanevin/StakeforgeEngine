// Copyright (c) 2025 Inan Evin

#include "mesh_raw.hpp"
#include "mesh.hpp"
#include "primitive.hpp"
#include "memory/chunk_allocator.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{

	void mesh_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << sid;
		stream << node_index;
		stream << primitives_static;
		stream << primitives_skinned;
	}

	void mesh_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> sid;
		stream >> node_index;
		stream >> primitives_static;
		stream >> primitives_skinned;
	}

}

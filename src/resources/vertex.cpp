// Copyright (c) 2025 Inan Evin
#include "vertex.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	void vertex_static::serialize(ostream& stream) const
	{
		stream << pos;
		stream << normal;
		stream << tangent;
		stream << uv;
	}
	void vertex_static::deserialize(istream& stream)
	{
		stream >> pos;
		stream >> normal;
		stream >> tangent;
		stream >> uv;
	}

	void vertex_skinned::serialize(ostream& stream) const
	{
		stream << pos;
		stream << normal;
		stream << tangent;
		stream << uv;
		stream << bone_weights;
		stream << bone_indices;
	}
	void vertex_skinned::deserialize(istream& stream)
	{
		stream >> pos;
		stream >> normal;
		stream >> tangent;
		stream >> uv;
		stream >> bone_weights;
		stream >> bone_indices;
	}
}

// Copyright (c) 2025 Inan Evin

#include "primitive_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	void primitive_static_raw::serialize(ostream& stream) const
	{
		stream << material_index;
		stream << vertices;
		stream << indices;
	}
	void primitive_static_raw::deserialize(istream& stream)
	{
		stream >> material_index;
		stream >> vertices;
		stream >> indices;
	}
	void primitive_skinned_raw::serialize(ostream& stream) const
	{
		stream << material_index;
		stream << vertices;
		stream << indices;
	}
	void primitive_skinned_raw::deserialize(istream& stream)
	{
		stream >> material_index;
		stream >> vertices;
		stream >> indices;
	}
}
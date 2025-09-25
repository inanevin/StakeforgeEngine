// Copyright (c) 2025 Inan Evin

#include "common_skin.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"
namespace SFG
{
	void skin_joint::serialize(ostream& stream) const
	{
		stream << model_node_index;
		stream << inverse_bind_matrix;
	}

	void skin_joint::deserialize(istream& stream)
	{
		stream >> model_node_index;
		stream >> inverse_bind_matrix;
	}
}
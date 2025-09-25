// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/string_id.hpp"
#include "data/string.hpp"
#include "primitive_raw.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct mesh_raw
	{
		string						  name		 = "";
		string_id					  sid		 = 0;
		uint16						  node_index = 0;
		vector<primitive_static_raw>  primitives_static;
		vector<primitive_skinned_raw> primitives_skinned;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};
}

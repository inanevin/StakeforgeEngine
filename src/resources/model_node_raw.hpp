// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "math/matrix4x3.hpp"
#include "data/string.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct model_node_raw
	{
		string	  name		   = "";
		int16	  parent_index = -1;
		int16	  mesh_index   = -1;
		matrix4x3 local_matrix = {};

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};
}

// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "primitive.hpp"

namespace SFG
{
	struct mesh
	{
		uint16					  node_index = 0;
		vector<primitive_static>  primitives_static;
		vector<primitive_skinned> primitives_skinned;
	};

}
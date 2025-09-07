// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "math/matrix4x4.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	struct skin_joint
	{
		uint16	  model_node_index	  = 0;
		matrix4x4 inverse_bind_matrix = {};
	};
	struct skin
	{
#ifdef SFG_TOOLMODE
		string name = "";
#endif
		vector<skin_joint> joints;
		uint16			   root_joint = 0;
	};
}
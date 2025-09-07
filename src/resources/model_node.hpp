// Copyright (c) 2025 Inan Evin
#pragma once
#include "math/matrix4x4.hpp"
#include "common/size_definitions.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	struct model_node
	{
#ifdef SFG_TOOLMODE
		string name = "";
#endif
		int16	  parent_index = -1;
		int16	  mesh_index   = -1;
		matrix4x4 local_matrix = {};
	};
}
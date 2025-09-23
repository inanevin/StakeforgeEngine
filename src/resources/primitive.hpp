// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "memory/chunk_handle.hpp"
#include "vertex.hpp"
#include "data/vector.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace SFG
{

	struct primitive_static_loaded
	{
		int16					material_index = -1;
		vector<vertex_static>	vertices;
		vector<primitive_index> indices;
	};

	struct primitive_skinned_loaded
	{
		int16					material_index = -1;
		vector<vertex_skinned>	vertices;
		vector<primitive_index> indices;
	};

	struct primitive_runtime
	{
		uint32 vertex_start = 0;
		uint32 index_start	= 0;
	};

	struct primitive
	{
		primitive_runtime runtime		 = {};
		int16			  material_index = -1;
		chunk_handle32	  vertices;
		chunk_handle32	  indices;
	};

}

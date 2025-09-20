// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "vertex.hpp"

namespace SFG
{
	struct prim_runtime
	{
		uint32 vertex_start = 0;
		uint32 index_start	= 0;
	};

	struct primitive_static
	{
		prim_runtime			runtime		   = {};
		int16					material_index = 0;
		vector<vertex_static>	vertices	   = {};
		vector<primitive_index> indices		   = {};
	};

	struct primitive_skinned
	{
		prim_runtime			runtime		   = {};
		int16					material_index = 0;
		vector<vertex_skinned>	vertices	   = {};
		vector<primitive_index> indices		   = {};
	};
}
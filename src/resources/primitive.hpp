// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "memory/chunk_handle.hpp"

namespace SFG
{
	struct primitive_runtime
	{
		uint32 vertex_start = 0;
		uint32 index_start	= 0;
	};

	struct primitive
	{
		primitive_runtime runtime		 = {};
		uint16			  material_index = 0;
		chunk_handle32	  vertices;
		chunk_handle32	  indices;
		uint32			  indices_count = 0;
	};

}

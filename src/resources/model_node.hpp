// Copyright (c) 2025 Inan Evin
#pragma once
#include "math/matrix4x3.hpp"
#include "common/size_definitions.hpp"
#include "memory/chunk_handle.hpp"
#include "world/common_world.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	class chunk_allocator32;

	struct model_node_loaded
	{
		string	  name		   = "";
		int16	  parent_index = -1;
		int16	  mesh_index   = -1;
		matrix4x3 local_matrix = {};
	};

	struct model_node
	{
		chunk_handle32 name			= {};
		int16		   parent_index = -1;
		int16		   mesh_index	= -1;
		matrix4x3	   local_matrix = {};

		void create_from_loaded(const model_node_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);
	};
}
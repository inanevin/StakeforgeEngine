// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "world/common_world.hpp"
#include "memory/chunk_handle.hpp"
#include "primitive.hpp"
#include "data/string.hpp"

namespace SFG
{

	class chunk_allocator32;

	struct mesh_loaded
	{
		string							 name		= "";
		uint16							 node_index = 0;
		vector<primitive_static_loaded>	 primitives_static;
		vector<primitive_skinned_loaded> primitives_skinned;
	};

	struct mesh
	{
		uint16		   node_index = 0;
		chunk_handle32 name;
		chunk_handle32 primitives_static;
		chunk_handle32 primitives_skinned;

		void create_from_loaded(const mesh_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);
	};

}
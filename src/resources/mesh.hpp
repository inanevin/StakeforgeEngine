// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "resources/common_resources.hpp"
#include "world/common_world.hpp"
#include "memory/chunk_handle.hpp"
#include "primitive.hpp"
#include "data/string.hpp"
#include "data/string_id.hpp"

namespace SFG
{

	class chunk_allocator32;

	struct mesh_loaded
	{
		string							 name		= "";
		string_id						 sid		= 0;
		uint16							 node_index = 0;
		vector<primitive_static_loaded>	 primitives_static;
		vector<primitive_skinned_loaded> primitives_skinned;
	};

	class mesh
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_mesh;

		inline chunk_handle32 get_primitives_static() const
		{
			return _primitives_static;
		}

		inline chunk_handle32 get_primitives_skinned() const
		{
			return _primitives_skinned;
		}

		inline uint16 get_primitives_static_count() const
		{
			return _primitives_static_count;
		}

		inline uint16 get_primitives_skinned_count() const
		{
			return _primitives_skinned_count;
		}

	private:
		friend class model;

		void create_from_loaded(const mesh_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

	private:
		uint16		   _node_index = 0;
		chunk_handle32 _name;
		chunk_handle32 _primitives_static;
		chunk_handle32 _primitives_skinned;
		uint16		   _primitives_static_count	 = 0;
		uint16		   _primitives_skinned_count = 0;
	};

}
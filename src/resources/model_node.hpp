// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "math/matrix4x3.hpp"
#include "memory/chunk_handle.hpp"

namespace SFG
{
	class chunk_allocator32;
	struct model_node_raw;

	class model_node
	{
	public:
		void create_from_raw(const model_node_raw& raw, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

		inline chunk_handle32 get_name() const
		{
			return _name;
		}

		inline int16 get_parent_index() const
		{
			return _parent_index;
		}

		inline int16 get_mesh_index() const
		{
			return _mesh_index;
		}

		inline const matrix4x3& get_local_matrix() const
		{
			return _local_matrix;
		}

	private:
		chunk_handle32 _name		 = {};
		int16		   _parent_index = -1;
		int16		   _mesh_index	 = -1;
		matrix4x3	   _local_matrix = {};
	};
}
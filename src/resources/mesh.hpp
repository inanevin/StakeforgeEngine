// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "resources/common_resources.hpp"
#include "memory/chunk_handle.hpp"

namespace SFG
{
	class chunk_allocator32;

	struct mesh_raw;

	class mesh
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_mesh;

		void create_from_raw(const mesh_raw& raw, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

		inline chunk_handle32 get_primitives_static() const
		{
			return _primitives_static;
		}

		inline chunk_handle32 get_primitives_skinned() const
		{
			return _primitives_skinned;
		}

		inline chunk_handle32 get_material_indices() const
		{
			return _material_indices;
		}

		inline uint16 get_primitives_static_count() const
		{
			return _primitives_static_count;
		}

		inline uint16 get_primitives_skinned_count() const
		{
			return _primitives_skinned_count;
		}

		inline uint16 get_node_index() const
		{
			return _node_index;
		}

		inline uint16 get_material_count() const
		{
			return _material_count;
		}

	private:
		friend class model;

	private:
		uint16		   _node_index	   = 0;
		uint16		   _material_count = 0;
		chunk_handle32 _name;
		chunk_handle32 _primitives_static;
		chunk_handle32 _primitives_skinned;
		chunk_handle32 _material_indices; // original indices into the loaded model.
		uint16		   _primitives_static_count	 = 0;
		uint16		   _primitives_skinned_count = 0;
	};

}
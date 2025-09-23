// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "math/matrix4x4.hpp"
#include "world/common_world.hpp"
#include "memory/chunk_handle.hpp"
#include "data/string_id.hpp"
#include "resources/common_resources.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	class chunk_allocator32;

	struct skin_joint
	{
		uint16	  model_node_index	  = 0;
		matrix4x4 inverse_bind_matrix = {};
	};
#ifdef SFG_TOOLMODE

	struct skin_loaded
	{
		string			   name = "";
		string_id		   sid	= 0;
		vector<skin_joint> joints;
		int16			   root_joint = -1;
	};
#endif

	class skin
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_skin;

#ifdef SFG_TOOLMODE
		void create_from_loaded(const skin_loaded& loaded, chunk_allocator32& alloc);
#endif

		void destroy(chunk_allocator32& alloc);

	private:
		chunk_handle32 _name;
		chunk_handle32 _joints;
		uint16		   _joints_count = 0;
		int16		   _root		 = -1;
	};
}
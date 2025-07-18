// Copyright (c) 2025 Inan Evin
#pragma once

#include "math/vector3.hpp"
#include "math/vector2.hpp"

namespace Game
{
	struct vertex
	{
		vector3 po = vector3();
		vector2 uv = vector2();
	};

	typedef uint32 index;

	struct mesh_data
	{
		uint32 vtx_start = 0;
		uint32 vtx_count = 0;
		uint32 idx_start = 0;
		uint32 idx_count = 0;
	};

	struct mesh_static
	{
		mesh_data data;
	};
}
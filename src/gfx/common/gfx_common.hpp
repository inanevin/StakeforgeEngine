// Copyright (c) 2025 Inan Evin
#pragma once

#include "math/vector3.hpp"
#include "math/vector2.hpp"
#include "math/vector2ui16.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace SFG
{
	struct vertex
	{
		vector3 po = vector3();
		vector2 uv = vector2();
	};

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

	struct semaphore_data
	{
		gfx_id semaphore = 0;
		uint64 value	 = 0;
	};

	struct texture_buffer
	{
		uint8*		pixels = nullptr;
		vector2ui16 size   = vector2ui16::zero;
		uint8		bpp	   = 0;
	};

	struct descriptor_handle
	{
		size_t cpu	 = NULL;
		uint64 gpu	 = NULL;
		uint32 index = 0;
		uint32 count = 0;
	};

}
// Copyright (c) 2025 Inan Evin
#pragma once
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/vector4i.hpp"

namespace SFG
{
	struct vertex_static
	{
		vector3 pos	   = vector3::zero;
		vector3 normal = vector3::zero;
		vector2 uv	   = vector2::zero;
	};

	struct vertex_skinned
	{
		vector3 pos			 = vector3::zero;
		vector3 normal		 = vector3::zero;
		vector2 uv			 = vector2::zero;
		vector4 bone_weights = vector4::zero;
	};
}
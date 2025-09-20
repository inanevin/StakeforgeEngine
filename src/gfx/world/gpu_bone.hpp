// Copyright (c) 2025 Inan Evin

#pragma once
#include "math/matrix4x3.hpp"

namespace SFG
{
#define MAX_GPU_BONES 256

	struct gpu_bone
	{
		matrix4x3 mat = matrix4x3::identity;
	};
}

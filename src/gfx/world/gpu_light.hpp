// Copyright (c) 2025 Inan Evin

#pragma once
#include "math/vector4.hpp"

namespace SFG
{
#define MAX_GPU_LIGHTS 32

	struct gpu_light
	{
		vector4 color = vector4::one;
	};
}

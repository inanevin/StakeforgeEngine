// Copyright (c) 2025 Inan Evin
#pragma once

#include "math/vector2ui16.hpp"

namespace SFG
{
	struct texture_buffer
	{
		uint8*		pixels = nullptr;
		vector2ui16 size   = vector2ui16::zero;
		uint8		bpp	   = 0;
	};

}
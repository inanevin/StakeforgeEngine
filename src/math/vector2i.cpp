// Copyright (c) 2025 Inan Evin

#include "vector2i.hpp"
#include "vector2ui.hpp"
#include "math.hpp"

namespace SFG
{
	vector2i vector2i::zero = vector2i(0, 0);
	vector2i vector2i::one	= vector2i(1, 1);

	vector2i::vector2i(const vector2ui& v)
	{
		x = static_cast<int32>(v.x);
		y = static_cast<int32>(v.y);
	}

	vector2i vector2i::clamp(const vector2i& v, const vector2i& min, const vector2i& max)
	{
		return vector2i(math::clamp(v.x, min.x, max.x), math::clamp(v.y, min.y, max.y));
	}

}
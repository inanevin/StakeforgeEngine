// Copyright (c) 2025 Inan Evin

#include "vector2i16.hpp"
#include "math.hpp"

namespace SFG
{
	vector2i16 vector2i16::zero = vector2i16(0, 0);
	vector2i16 vector2i16::one	= vector2i16(1, 1);

	vector2i16 vector2i16::clamp(const vector2i16& v, const vector2i16& min, const vector2i16& max)
	{
		return vector2i16(math::clamp(v.x, min.x, max.x), math::clamp(v.y, min.y, max.y));
	}

}
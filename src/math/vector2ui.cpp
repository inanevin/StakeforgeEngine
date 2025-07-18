// Copyright (c) 2025 Inan Evin

#include "vector2ui.hpp"
#include "vector2i.hpp"

namespace Game
{
	vector2ui vector2ui::zero = vector2ui(0, 0);
	vector2ui vector2ui::one  = vector2ui(1, 1);

	vector2ui::vector2ui(const vector2i& v)
	{
		x = static_cast<uint32>(v.x);
		x = static_cast<uint32>(v.y);
	}
}
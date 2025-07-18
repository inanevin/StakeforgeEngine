// Copyright (c) 2025 Inan Evin

#pragma once

#include <span>

namespace Game
{
	template <typename T> struct span
	{
		T*	   data = nullptr;
		size_t size = 0;
	};
}

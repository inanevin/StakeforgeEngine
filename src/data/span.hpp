// Copyright (c) 2025 Inan Evin

#pragma once

#include <span>

namespace SFG
{
	template <typename T> struct span
	{
		T*	   data = nullptr;
		size_t size = 0;
	};
}

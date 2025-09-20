// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"

namespace SFG
{
	template <typename T> struct chunk_handle
	{
		T head = 0;
		T size = 0;
	};
}

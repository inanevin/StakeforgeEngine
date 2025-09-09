// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"

namespace SFG
{
	template <typename T> struct pool_handle
	{
		T id;
		T generation;
	};
}

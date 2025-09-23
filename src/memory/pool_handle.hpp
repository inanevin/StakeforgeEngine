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

	struct pool_handle16
	{
		uint16 generation = 0;
		uint16 index	  = 0;

		bool operator==(const pool_handle16& other) const
		{
			return generation == other.generation && index == other.index;
		}

		bool is_null() const
		{
			return generation == 0;
		}
	};

}

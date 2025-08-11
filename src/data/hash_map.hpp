// Copyright (c) 2025 Inan Evin

#pragma once

#include <parallel_hashmap/phmap.h>

namespace SFG
{
	template <typename T, typename U> using hash_map		  = phmap::flat_hash_map<T, U>;
	template <typename T, typename U> using parallel_hash_map = phmap::parallel_flat_hash_map<T, U>;

}

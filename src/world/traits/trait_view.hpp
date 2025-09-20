// Copyright (c) 2025 Inan Evin

#pragma once

#include "memory/pool_allocator.hpp"
#include "world/common_world.hpp"

namespace SFG
{
	template <typename T, size_t N> class trait_view
	{
	public:
		pool_allocator_gen<T, world_id, N>& get_pool()
		{
			return _pool;
		}

	private:
		pool_allocator_gen<T, world_id, N> _pool;
	};

}

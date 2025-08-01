// Copyright (c) 2025 Inan Evin

#pragma once

#include <queue>

namespace Game
{
	template <typename T, typename Container = std::deque<T>> using queue = std::queue<T, Container>;

}

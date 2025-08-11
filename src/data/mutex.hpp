// Copyright (c) 2025 Inan Evin

#pragma once

#include <mutex>

namespace SFG
{
	typedef std::mutex mutex;

#define LOCK_GUARD(X) std::lock_guard<mutex> lock_guard(X)
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include <mutex>

namespace Game
{
	typedef std::mutex mutex;

#define LOCK_GUARD(X) std::lock_guard<mutex>##X_lock_guard(X)
}

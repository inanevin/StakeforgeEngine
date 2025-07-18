// Copyright (c) 2025 Inan Evin

#pragma once

namespace Game
{

#ifdef GAME_PLATFORM_WINDOWS
#define DBG_BRK __debugbreak();
#else
#define DBG_BRK __builtin_trap();
#endif

#ifdef GAME_DEBUG
#define GAME_ASSERT(x, ...)                                                                                                                                                                                                                                        \
	if (!(x)) { DBG_BRK }

#else
#define GAME_ASSERT(x, ...)
#endif

#define GAME_NOTIMPLEMENTED GAME_ASSERT(false, "Implementation missing!")

}
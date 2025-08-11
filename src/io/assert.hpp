// Copyright (c) 2025 Inan Evin

#pragma once

namespace SFG
{

#ifdef SFG_PLATFORM_WINDOWS
#define DBG_BRK __debugbreak();
#else
#define DBG_BRK __builtin_trap();
#endif

#ifdef SFG_DEBUG
#define SFG_ASSERT(x, ...)                                                                                                                                                                                                                                         \
	if (!(x))                                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                              \
		DBG_BRK                                                                                                                                                                                                                                                    \
	}

#else
#define SFG_ASSERT(x, ...)
#endif

#define SFG_NOTIMPLEMENTED SFG_ASSERT(false, "Implementation missing!")

}
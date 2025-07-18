// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef GAME_COMPILER_MSVC
#include <malloc.h>
#define GAME_ALIGNED_MALLOC(ALIGNMENT, SIZE) _aligned_malloc(SIZE, ALIGNMENT)
#define GAME_ALIGNED_FREE(...)				 _aligned_free(__VA_ARGS__)
#else
#include <cstdlib>
#define GAME_ALIGNED_MALLOC(...) std::aligned_alloc(__VA_ARGS__)
#define GAME_ALIGNED_FREE(...)	 std::free(__VA_ARGS__)
#endif

#include <memory>
#define GAME_MEMCPY(...)  memcpy(__VA_ARGS__)
#define GAME_MEMMOVE(...) memmove(__VA_ARGS__)
#define GAME_MEMSET(...)  memset(__VA_ARGS__)
#define GAME_MALLOC(...)  malloc(__VA_ARGS__)
#define GAME_FREE(...)	  free(__VA_ARGS__)

namespace Game
{

}

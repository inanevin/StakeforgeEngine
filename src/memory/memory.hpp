// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef SFG_COMPILER_MSVC
#include <malloc.h>
#define SFG_ALIGNED_MALLOC(ALIGNMENT, SIZE) _aligned_malloc(SIZE, ALIGNMENT)
#define SFG_ALIGNED_FREE(...)				_aligned_free(__VA_ARGS__)
#else
#include <cstdlib>
#define SFG_ALIGNED_MALLOC(...) std::aligned_alloc(__VA_ARGS__)
#define SFG_ALIGNED_FREE(...)	std::free(__VA_ARGS__)
#endif

#include <memory>
#define SFG_MEMCPY(...)	 memcpy(__VA_ARGS__)
#define SFG_MEMMOVE(...) memmove(__VA_ARGS__)
#define SFG_MEMSET(...)	 memset(__VA_ARGS__)
#define SFG_MALLOC(...)	 malloc(__VA_ARGS__)
#define SFG_FREE(...)	 free(__VA_ARGS__)

namespace Game
{

}

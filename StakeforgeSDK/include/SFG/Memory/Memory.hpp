/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifdef SFG_COMPILER_MSVC
#include <malloc.h>
#define SFG_ALIGNED_MALLOC(...) _aligned_malloc(__VA_ARGS__)
#define SFG_ALIGNED_FREE(...)	_aligned_free(__VA_ARGS__)
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

namespace SFG
{

} // namespace SFG

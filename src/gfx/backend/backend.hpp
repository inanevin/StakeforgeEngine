// Copyright (c) 2025 Inan Evin
#pragma once

#ifdef SFG_PLATFORM_WINDOWS
#include "gfx/backend/dx12/dx12_backend.hpp"
#else
#endif

namespace SFG
{
#ifdef SFG_PLATFORM_WINDOWS
	typedef dx12_backend gfx_backend;
#else

#endif

}
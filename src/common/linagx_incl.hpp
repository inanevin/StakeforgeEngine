// Copyright (c) 2025 Inan Evin

#pragma once

#include <LinaGX/LinaGX.hpp>

namespace LinaGX
{
	extern LinaGX::Instance* instance();
	extern void				 set_instance(LinaGX::Instance* s);
	extern LinaGX::Instance* s_lgxInstance;
}

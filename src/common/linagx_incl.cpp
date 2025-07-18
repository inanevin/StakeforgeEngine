// Copyright (c) 2025 Inan Evin

#include "linagx_incl.hpp"

namespace LinaGX
{
	LinaGX::Instance* s_lgxInstance = nullptr;
	LinaGX::Instance* instance()
	{
		return s_lgxInstance;
	}
	void set_instance(LinaGX::Instance* inst)
	{
		s_lgxInstance = inst;
	}
}
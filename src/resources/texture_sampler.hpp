// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace SFG
{

	class texture_sampler
	{
	public:
		~texture_sampler();

#ifdef SFG_TOOLMODE
		bool create_from_file(const char* file);
#endif

		void   destroy();
		gfx_id get_hw() const;

	private:
		gfx_id _hw;
	};

}

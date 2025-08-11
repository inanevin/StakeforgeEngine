// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"

namespace SFG
{
	class texture_sampler
	{
	public:
		~texture_sampler();

		// void create(const LinaGX::SamplerDesc& desc);
		// void destroy();

		inline uint32 get_hw() const
		{
			return _hw;
		}

	private:
		uint32 _hw = 0;
	};

}

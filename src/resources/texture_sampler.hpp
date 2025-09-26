// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "resources/common_resources.hpp"
#include <limits>

namespace SFG
{
	struct texture_sampler_raw;

	class texture_sampler
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_texture_sampler;

		~texture_sampler();

		void create_from_raw(const texture_sampler_raw& raw);
		void destroy();

		inline gfx_id get_hw() const
		{
			return _hw;
		}

	private:
		gfx_id _hw = std::numeric_limits<gfx_id>::max();
	};

}

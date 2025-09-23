// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{

	class texture_sampler
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_texture_sampler;

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

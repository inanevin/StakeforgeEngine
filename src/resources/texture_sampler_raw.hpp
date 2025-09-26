// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct texture_sampler_raw
	{
		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif

		sampler_desc desc = {};
	};
}

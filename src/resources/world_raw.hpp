// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct world_raw
	{
		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
		void save_to_file(const char* file);
#endif
	};
}

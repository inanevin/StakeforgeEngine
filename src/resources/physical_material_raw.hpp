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

	struct physical_material_raw
	{
		float restitution  = 0.0f;
		float friction	   = 0.0f;
		float angular_damp = 0.0f;
		float linear_damp  = 0.0f;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};
}

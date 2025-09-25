// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/static_vector.hpp"
#include "gfx/common/texture_buffer.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct texture_raw
	{
		string											name		   = "";
		uint8											texture_format = 0;
		uint8											gen_mips	   = 0;
		static_vector<texture_buffer, MAX_TEXTURE_MIPS> buffers;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};

}

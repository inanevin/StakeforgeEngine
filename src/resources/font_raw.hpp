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

	struct font_raw
	{
		string		 name		  = "";
		uint16		 point_size	  = 0;
		vector<char> font_data	  = {};
		uint8		 font_type	  = 0;
		int16		 sdf_padding  = 0;
		int16		 sdf_edge	  = 0;
		float		 sdf_distance = 0.0f;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};
}

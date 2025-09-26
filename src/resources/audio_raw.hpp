// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"
#include "data/string.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct audio_raw
	{
		string		 name	   = "";
		uint8		 is_stream = 0;
		vector<char> audio_data;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};
}

// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "common_skin.hpp"
#include "data/vector.hpp"
#include "data/string_id.hpp"
#include "data/string.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct skin_raw
	{
		string			   name = "";
		string_id		   sid	= 0;
		vector<skin_joint> joints;
		int16			   root_joint = -1;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};
}

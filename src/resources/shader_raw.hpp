// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/shader_description.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct shader_raw
	{
		string		   name		  = "";
		shader_desc	   desc		  = {};
		vector<string> defines	  = {};
		uint8		   is_skinned = 0;

		shader_raw(){};
		shader_raw(const shader_raw& temp_obj)			  = delete;
		shader_raw& operator=(const shader_raw& temp_obj) = delete;
		~shader_raw();

		void serialize(ostream& stream, const shader_desc& desc) const;
		void deserialize(istream& stream, shader_desc& desc);

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/ostream.hpp"
#include "data/static_vector.hpp"
#include "data/string_id.hpp"
#include "resources/common_resources.hpp"
#include "gfx/common/gfx_constants.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#include "vendor/nhlohmann/json.hpp"
#endif

namespace SFG
{
	class istream;

#ifdef SFG_TOOLMODE
	struct parameter_entry
	{
		string		   name;
		nlohmann::json value;
	};

	void to_json(nlohmann::json& j, const parameter_entry& p);
	void from_json(const nlohmann::json& j, parameter_entry& p);

#endif

	struct material_raw
	{
		ostream			  material_data = {};
		vector<string_id> shaders;
		vector<string_id> textures;
		uint8			  is_opaque	 = 0;
		uint8			  is_forward = 0;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

#ifdef SFG_TOOLMODE
		bool cook_from_file(const char* file);
#endif
	};
}

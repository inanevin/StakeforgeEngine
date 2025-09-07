// Copyright (c) 2025 Inan Evin
#include "descriptions.hpp"

#ifdef SFG_TOOLMODE
#include "io/assert.hpp"
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const sampler_desc& s)
	{
		j["anisotropy"] = s.anisotropy;
		j["min_lod"]	= s.min_lod;
		j["max_lod"]	= s.max_lod;
		j["flags"]		= s.flags.value();
	}
	void from_json(const nlohmann::json& j, sampler_desc& s)
	{
		s.anisotropy = j.value<uint32>("anisotropy", 0);
		s.min_lod	 = j.value<float>("min_lod", .0f);
		s.max_lod	 = j.value<float>("max_lod", .0f);
		s.flags		 = j.value<uint16>("flags", 0);
	}

#endif

}
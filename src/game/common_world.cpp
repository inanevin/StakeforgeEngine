// Copyright (c) 2025 Inan Evin

#include "common_world.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json.hpp"
#endif
namespace SFG
{

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const resource_ident& s)
	{
		j["path"] = s.path;
	}

	void from_json(const nlohmann::json& j, resource_ident& s)
	{
		s.path = j.value<string>("path", "");
	}

#endif
}
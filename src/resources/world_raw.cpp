// Copyright (c) 2025 Inan Evin

#include "world_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include "io/log.hpp"
#include "project/engine_data.hpp"
#include "gui/vekt.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
	void world_raw::serialize(ostream& stream) const
	{
	}

	void world_raw::deserialize(istream& stream)
	{
	}

#ifdef SFG_TOOLMODE
	bool world_raw::cook_from_file(const char* path)
	{
		if (!file_system::exists(path))
		{
			SFG_ERR("File don't exist! {0}", path);
			return false;
		}

		try
		{
			std::ifstream f(path);
			json		  json_data = json::parse(f);
			f.close();

			if (!file_system::exists(source.c_str()))
			{
				SFG_ERR("File don't exist! {0}", source.c_str());
				return false;
			}
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading world: {0}", e.what());
			return false;
		}

		SFG_INFO("Created world from file: {0}", path);
		return true;
	}

#endif
}

// Copyright (c) 2025 Inan Evin

#include "physical_material_raw.hpp"
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
	void physical_material_raw::serialize(ostream& stream) const
	{
		stream << restitution;
		stream << friction;
		stream << angular_damp;
		stream << linear_damp;
	}

	void physical_material_raw::deserialize(istream& stream)
	{
		stream >> restitution;
		stream >> friction;
		stream >> angular_damp;
		stream >> linear_damp;
	}

#ifdef SFG_TOOLMODE
	bool physical_material_raw::cook_from_file(const char* path)
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

			restitution	 = json_data.value<float>("restitution", 0.0f);
			friction	 = json_data.value<float>("restitution", 0.2f);
			angular_damp = json_data.value<float>("restitution", 0.05f);
			linear_damp	 = json_data.value<float>("restitution", 0.05f);
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading physical material: {0}", e.what());
			return false;
		}

		SFG_INFO("Created physical material from file: {0}", path);
		return true;
	}

#endif
}

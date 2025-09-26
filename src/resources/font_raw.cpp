// Copyright (c) 2025 Inan Evin

#include "font_raw.hpp"
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
	void font_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << point_size;
		stream << font_data;
		stream << font_type;
		stream << sdf_padding;
		stream << sdf_edge;
		stream << sdf_distance;
	}

	void font_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> point_size;
		stream >> font_data;
		stream >> font_type;
		stream >> sdf_padding;
		stream >> sdf_edge;
		stream >> sdf_distance;
	}

#ifdef SFG_TOOLMODE
	bool font_raw::cook_from_file(const char* path)
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

			name				= json_data.value<string>("source", "");
			const string source = engine_data::get().get_working_dir() + name;
			if (!file_system::exists(source.c_str()))
			{
				SFG_ERR("File don't exist! {0}", source.c_str());
				return false;
			}

			point_size = json_data.value<uint16>("point_size", 0);

			if (point_size == 0 || point_size > 120)
			{
				SFG_ERR("Invalid font point size! {0}", point_size);
				return false;
			}

			file_system::read_file_as_vector(source.c_str(), font_data);
			if (font_data.empty())
			{
				SFG_ERR("Invalid font data!");
				return false;
			}

			const string type = json_data.value<string>("type", "");

			if (type.compare("normal") == 0)
				font_type = static_cast<uint8>(vekt::font_type::normal);
			else if (type.compare("sdf") == 0)
				font_type = static_cast<uint8>(vekt::font_type::sdf);
			else if (type.compare("lcd") == 0)
				font_type = static_cast<uint8>(vekt::font_type::lcd);

			sdf_padding	 = json_data.value<int16>("sdf_padding", 0);
			sdf_edge	 = json_data.value<int16>("sdf_edge", 0);
			sdf_distance = json_data.value<float>("sdf_distance", 0);
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading font: {0}", e.what());
			return false;
		}

		SFG_INFO("Created font from file: {0}", path);
		return true;
	}

#endif
}

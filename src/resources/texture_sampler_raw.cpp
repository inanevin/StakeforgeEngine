// Copyright (c) 2025 Inan Evin

#include "texture_sampler_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include "io/log.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
	void texture_sampler_raw::serialize(ostream& stream) const
	{
		stream << desc.anisotropy;
		stream << desc.lod_bias;
		stream << desc.min_lod;
		stream << desc.max_lod;
		stream << desc.flags.value();
	}

	void texture_sampler_raw::deserialize(istream& stream)
	{
		uint8 val = 0;
		stream >> desc.anisotropy;
		stream >> desc.lod_bias;
		stream >> desc.min_lod;
		stream >> desc.max_lod;
		stream >> val;
		desc.flags = val;
	}

#ifdef SFG_TOOLMODE
	bool texture_sampler_raw::cook_from_file(const char* path)
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
			desc = json_data;
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading texture sampler: {0}", e.what());
			return false;
		}

		SFG_INFO("Created texture sampler from file: {0}", path);
		return true;
	}

#endif
}

// Copyright (c) 2025 Inan Evin
#ifdef SFG_TOOLMODE

#include "engine_data.hpp"
#include "app/debug_console.hpp"
#include "io/log.hpp"
#include "io/file_system.hpp"
#include "platform/process.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;

#define ENGINE_DATA_PATH "engine.stkfrg"

namespace SFG
{
	void engine_data::init()
	{
		debug_console::get()->register_console_function<>("ed_set_work_dir", [this]() {
			const string dir = process::select_folder("Select working directory") + "/";
			if (file_system::exists(dir.c_str()))
			{
				_working_dir = dir;
				save();
			}
		});

		debug_console::get()->register_console_function<>("ed_report", [this]() { report(); });

		if (file_system::exists(ENGINE_DATA_PATH))
			load();
		else
			save();

		if (!file_system::exists(_working_dir.c_str()))
		{
			_working_dir = process::select_folder("Select working directory") + "/";
			SFG_ASSERT(file_system::exists(_working_dir.c_str()));
			save();
		}
	}

	void engine_data::uninit()
	{
		save();
	}

	void engine_data::load()
	{
		std::ifstream f(ENGINE_DATA_PATH);
		json		  data = json::parse(f);
		_working_dir	   = data["working_dir"];
		_last_world		   = data.value("last_world", "");
		f.close();
	}

	void engine_data::save()
	{
		json j;
		j["working_dir"] = _working_dir;
		j["last_world"]	 = _last_world;

		std::ofstream file(ENGINE_DATA_PATH);
		if (file.is_open())
		{
			file << j.dump(4);
			file.close();
		}
	}

	void engine_data::report()
	{
		SFG_WARN("********** engine_data report bgn ********** ");
		SFG_INFO("working dir: {0}", _working_dir);
		SFG_WARN("********** engine_data report end ********** ");
	}
}

#endif

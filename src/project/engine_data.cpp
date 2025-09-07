// Copyright (c) 2025 Inan Evin

#include "engine_data.hpp"
#include "app/debug_console.hpp"
#include "io/log.hpp"
#include "io/file_system.hpp"

#ifdef SFG_TOOLMODE
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

#define ENGINE_DATA_PATH "engine.stkfrg"

namespace SFG
{
	void engine_data::init()
	{
		debug_console::get()->register_console_function<string>("ed_set_work_dir", [this](const string& dir) { _working_dir = dir; });
		debug_console::get()->register_console_function<>("ed_report", [this]() { report(); });
		debug_console::get()->register_console_function<>("ed_save", [this]() { save(); });
		debug_console::get()->register_console_function<>("ed_load", [this]() { load(); });

#ifdef SFG_TOOLMODE
		if (file_system::exists(ENGINE_DATA_PATH))
			load();
		else
			save();
#endif
	}

	void engine_data::uninit()
	{
	}

	void engine_data::load()
	{
#ifdef SFG_TOOLMODE
		std::ifstream f(ENGINE_DATA_PATH);
		json		  data = json::parse(f);
		_working_dir	   = data["working_dir"];
		_last_world		   = data.value("last_world", "");
		f.close();
#endif
	}

	void engine_data::save()
	{
#ifdef SFG_TOOLMODE

		json j;
		j["working_dir"] = _working_dir;
		j["last_world"]	 = _last_world;

		std::ofstream file(ENGINE_DATA_PATH);
		if (file.is_open())
		{
			file << j.dump(4);
			file.close();
		}
#endif
	}

	void engine_data::report()
	{
		SFG_WARN("********** engine_data report bgn ********** ");
		SFG_INFO("working dir: {0}", _working_dir);
		SFG_WARN("********** engine_data report end ********** ");
	}
}

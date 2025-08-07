// Copyright (c) 2025 Inan Evin
#include "scene_context.hpp"
#include "io/file_system.hpp"
#include "io/log.hpp"
#include "app/debug_console.hpp"
#include "gltf_definitions.hpp"
#include <glaze/glaze.hpp>

namespace Game
{

	void scene_context::init()
	{
		debug_console::get()->register_console_function<const char*>("sc_add_gltf", std::bind(&scene_context::add_gltf_from_file, this, std::placeholders::_1));
	}

	void scene_context::uninit()
	{
	}

	void scene_context::add_gltf_from_file(const char* file)
	{

		if (file == nullptr || !file_system::exists(file))
		{
			GAME_ERR("scene_context::add_gltf_from_file() -> file not found! {0}", file);
			return;
		}

		const string str = file_system::read_file_as_string(file);
		GAME_PROG("scene_context::add_gtlf_from_file() -> Loading gltf file: {0}", file);

		gltf::gltf_json gltf_data = {};
		auto			pe		  = glz::read_json(gltf_data, str);
		if (pe)
		{
			const std::string err = glz::format_error(pe, str);
			GAME_ERR("scene_context::add_gtlf_from_file() -> Failed reading gltf json! {0}", err);
			return;
		}
	}
}
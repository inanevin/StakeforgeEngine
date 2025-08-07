// Copyright (c) 2025 Inan Evin
#pragma once

namespace Game
{
	class scene_context
	{
	public:
		void init();
		void uninit();

		void add_gltf_from_file(const char* file);
	};
}
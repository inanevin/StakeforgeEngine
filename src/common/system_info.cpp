// Copyright (c) 2025 Inan Evin

#include "system_info.hpp"

namespace Game
{
#ifdef GAME_DEBUG
	std::thread::id thread_info::_thread_id_render;
	std::thread::id thread_info::_thread_id_main;
#endif
}

// Copyright (c) 2025 Inan Evin

#include "system_info.hpp"

namespace Game
{
#ifdef GAME_DEBUG
	std::thread::id thread_info::s_thread_id_render;
	std::thread::id thread_info::s_thread_id_main;
	bool			thread_info::s_is_init;
#endif

	atomic<double> frame_info::s_main_thread_time_milli	  = 0;
	atomic<double> frame_info::s_render_thread_time_milli = 0;
	atomic<double> frame_info::s_present_time_milli		  = 0;
	atomic<uint32> frame_info::s_fps					  = 0;
}

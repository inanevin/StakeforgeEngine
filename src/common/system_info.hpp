// Copyright (c) 2025 Inan Evin

#pragma once
#include "size_definitions.hpp"
#include "data/atomic.hpp"

#ifdef GAME_DEBUG
#include <thread>
#include "io/assert.hpp"
#endif

namespace Game
{
#ifdef GAME_DEBUG

	class thread_info
	{
	public:
		static inline std::thread::id get_thread_id_main()
		{
			return _thread_id_main;
		}

		static inline std::thread::id get_thread_id_render()
		{
			return _thread_id_render;
		}

	private:
		friend class game_app;

		static inline void register_thread_main(std::thread::id id)
		{
			_thread_id_main = id;
		}

		static inline void register_thread_render(std::thread::id id)
		{
			_thread_id_render = id;
		}

	private:
		static std::thread::id _thread_id_render;
		static std::thread::id _thread_id_main;
	};

	class frame_info
	{
	public:
		static double get_main_thread_time_milli()
		{
			return s_main_thread_time_milli.load();
		}

		static double get_render_thread_time_milli()
		{
			return s_render_thread_time_milli.load();
		}

		static uint32 get_fps()
		{
			return s_fps.load();
		}

	private:
		friend class game_app;

		static atomic<double> s_main_thread_time_milli;
		static atomic<double> s_render_thread_time_milli;
		static atomic<uint32> s_fps;
	};

#define REGISTER_THREAD_MAIN()	 thread_info::register_thread_main(std::this_thread::get_id())
#define REGISTER_THREAD_RENDER() thread_info::register_thread_render(std::this_thread::get_id())
#define VERIFY_THREAD_MAIN()	 GAME_ASSERT(thread_info::get_thread_id_main() == std::this_thread::get_id())
#define VERIFY_THREAD_RENDER()	 GAME_ASSERT(thread_info::get_thread_id_render() == std::this_thread::get_id())
#else
#define REGISTER_THREAD_MAIN()
#define REGISTER_RENDER_THREAD()
#define VERIFY_THREAD_MAIN()
#define VERIFY_THREAD_RENDER()
#endif

}

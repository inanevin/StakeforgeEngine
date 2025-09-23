// Copyright (c) 2025 Inan Evin

#pragma once
#include "size_definitions.hpp"
#include "data/atomic.hpp"

#ifdef SFG_DEBUG
#include <thread>
#include "io/assert.hpp"
#endif

namespace SFG
{
#ifdef SFG_DEBUG

	class thread_info
	{
	public:
		static inline std::thread::id get_thread_id_main()
		{
			return s_thread_id_main;
		}

		static inline std::thread::id get_thread_id_render()
		{
			return s_thread_id_render;
		}

		static inline bool get_is_init()
		{
			return s_is_init;
		}

	private:
		friend class game_app;

		static inline void register_thread_main(std::thread::id id)
		{
			s_thread_id_main = id;
		}

		static inline void register_thread_render(std::thread::id id)
		{
			s_thread_id_render = id;
		}

		static inline void set_is_init(bool is_init)
		{
			s_is_init = is_init;
		}

	private:
		static std::thread::id s_thread_id_render;
		static std::thread::id s_thread_id_main;
		static bool			   s_is_init;
	};

#define REGISTER_THREAD_MAIN()	 thread_info::register_thread_main(std::this_thread::get_id())
#define REGISTER_THREAD_RENDER() thread_info::register_thread_render(std::this_thread::get_id())
#define SET_INIT(IS_INIT)		 thread_info::set_is_init(IS_INIT)
#define VERIFY_THREAD_MAIN()	 SFG_ASSERT(thread_info::get_thread_id_main() == std::this_thread::get_id())
#define VERIFY_THREAD_RENDER()	 SFG_ASSERT(thread_info::get_thread_id_render() == std::this_thread::get_id())
#define VERIFY_INIT()			 SFG_ASSERT(thread_info::get_is_init())
#define IS_RENDER_THREAD()		 thread_info::get_thread_id_main() == std::this_thread::get_id()
#else
#define REGISTER_THREAD_MAIN()
#define REGISTER_THREAD_RENDER()
#define SET_INIT(IS_INIT)
#define VERIFY_THREAD_MAIN()
#define VERIFY_THREAD_RENDER()
#define VERIFY_INIT()
#endif

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

		static double get_present_time_micro()
		{
			return s_present_time_micro.load();
		}

		static double get_present_time_milli()
		{
			return s_present_time_milli.load();
		}

		static uint32 get_fps()
		{
			return s_fps.load();
		}

		static uint64 get_frame()
		{
			return s_frame.load();
		}

		static uint64 get_render_frame()
		{
			return s_render_frame.load();
		}

		static inline bool get_is_render_acitve()
		{
			return s_is_render_active;
		}

	private:
		friend class game_app;
		friend class renderer;

	private:
		static inline void set_render_joined(bool b)
		{
			s_is_render_active = b;
		}

		static atomic<double> s_main_thread_time_milli;
		static atomic<double> s_render_thread_time_milli;
		static atomic<double> s_present_time_micro;
		static atomic<double> s_present_time_milli;
		static atomic<uint32> s_fps;
		static atomic<uint64> s_frame;
		static atomic<uint64> s_render_frame;
		static bool			  s_is_render_active;
	};

#define VERIFY_RENDER_NOT_RUNNING()					 SFG_ASSERT(!frame_info::get_is_render_acitve())
#define VERIFY_RENDER_NOT_RUNNING_OR_RENDER_THREAD() SFG_ASSERT(thread_info::get_thread_id_render() == std::this_thread::get_id() || !frame_info::get_is_render_acitve())
}

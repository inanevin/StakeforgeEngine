// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "platform/window.hpp"
#include "gfx/frame_processor.hpp"
#include "gfx/renderer.hpp"
#include "data/binary_semaphore.hpp"
#include "data/atomic.hpp"
#include <thread>

namespace SFG
{
	class window;
	struct window_event;
	class vector2ui;

	class game_app
	{
	public:
		enum game_app_flags
		{
			gaf_should_exit = 1 << 0,
		};

		void init(const vector2ui16& render_target_size);
		void uninit();
		void tick();
		void join_render();

#ifdef SFG_TOOLMODE
		void resize_render_target(const vector2ui16& render_target_size);
#endif

		inline bool get_should_exit() const
		{
			return _flags.is_set(gaf_should_exit);
		}

#ifndef SFG_TOOLMODE
		inline window& get_main_window()
		{
			return _main_window;
		}
#endif

	private:
		void kick_off_render();

#ifdef SFG_TOOLMODE
		void pipe_read();
#endif
		void render_loop();
		void tick_game_state(float delta_time);
		void on_window_event(const window_event& ev);

	private:
#ifndef SFG_TOOLMODE
		window _main_window = {};
#else
		vector2ui16 _render_target_size = vector2ui16::zero;
#endif
		renderer	_renderer = {};
		std::thread _render_thread;
#ifdef SFG_TOOLMODE
		std::thread _pipe_read_thread;
#endif
		binary_semaphore _frame_available_semaphore{0};
		uint8			 _update_render_frame_index	 = 0;
		atomic<uint8>	 _current_render_frame_index = 0;
		atomic<uint8>	 _should_close;
		atomic<uint8>	 _render_joined;
		bitmask<uint8>	 _flags = 0;
	};
}
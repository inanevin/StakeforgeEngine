// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "platform/window.hpp"
#include "gfx/frame_processor.hpp"
#include "gfx/renderer.hpp"
#include "data/binary_semaphore.hpp"
#include "data/atomic.hpp"
#include <thread>

namespace Game
{
	class window;
	struct window_event;
	class vector2ui;

	class game_app
	{
	public:
		void init();
		void uninit();
		void tick();
		void tick_game_state(float delta_time);
		void on_window_event(const window_event& ev);
		void join_render();

		inline bool get_should_exit() const
		{
			return _should_exit;
		}

		inline window& get_main_window()
		{
			return _main_window;
		}

	private:
		void kick_off_render();
		void render_loop();

	private:
		window			 _main_window = {};
		renderer		 _renderer	  = {};
		std::thread		 _render_thread;
		binary_semaphore _frame_available_semaphore{0};
		uint8			 _update_render_frame_index	 = 0;
		atomic<uint8>	 _current_render_frame_index = 0;
		atomic<uint8>	 _should_close;
		atomic<uint8>	 _render_joined;
		bool			 _should_exit = false;
	};
}
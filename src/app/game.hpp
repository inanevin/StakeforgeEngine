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
	class world;
	class world_renderer;

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

		inline bool get_should_exit() const
		{
			return _flags.is_set(gaf_should_exit);
		}

		inline window& get_main_window()
		{
			return _main_window;
		}

	private:
		void kick_off_render();

		void render_loop();
		void tick_game_state(float delta_time);
		void on_window_event(const window_event& ev);

	private:
		window			 _main_window	 = {};
		renderer		 _renderer		 = {};
		world*			 _world			 = nullptr;
		world_renderer*	 _world_renderer = nullptr;
		std::thread		 _render_thread;
		binary_semaphore _frame_available_semaphore{0};
		uint8			 _update_render_frame_index	 = 0;
		atomic<uint8>	 _current_render_frame_index = 0;
		atomic<uint8>	 _should_close;
		atomic<uint8>	 _render_joined;
		bitmask<uint8>	 _flags = 0;
	};
}
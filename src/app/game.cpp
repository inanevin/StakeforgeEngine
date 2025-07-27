// Copyright (c) 2025 Inan Evin
#include "game.hpp"
#include "platform/time.hpp"
#include "platform/process.hpp"
#include "memory/memory_tracer.hpp"
#include "common/system_info.hpp"
#include "gfx/common/render_data.hpp"
#include "gfx/backend/backend.hpp"
#include "io/log.hpp"
#include "debug_console.hpp"

namespace Game
{
#define GAME_DT 1.0f / 60.0f

	void game_app::init()
	{
		REGISTER_THREAD_MAIN();

		time::init();
		debug_console::init();

		PUSH_MEMORY_CATEGORY("Gfx");
		_main_window.create("Game", window_flags::wf_style_windowed | window_flags::wf_high_freq, vector2i16(0, 0), vector2ui16(1920, 1080));

		vector<monitor_info> out_infos;
		process::get_all_monitors(out_infos);

		_main_window.set_position(out_infos[1].position);
		_main_window.set_size(out_infos[1].work_size);

		_renderer.init(_main_window);
		POP_MEMORY_CATEGORY();

		_render_joined.store(1);
		kick_off_render();
	}

	void game_app::uninit()
	{
		join_render();

		time::uninit();
		debug_console::uninit();

		PUSH_MEMORY_CATEGORY("Gfx");
		_renderer.uninit();
		_main_window.destroy();
		POP_MEMORY_CATEGORY();
	}

	void game_app::tick()
	{
		const int64 FIXED_INTERVAL_US = (int64)1000000 / (int64)GAME_DT;
		int64		previous_time	  = time::get_cpu_microseconds();
		int64		accumulator		  = FIXED_INTERVAL_US;

		while (_should_close.load(std::memory_order_acquire) == 0)
		{
			const int64 current_time = time::get_cpu_microseconds();
			const int64 delta_micro	 = current_time - previous_time;
			previous_time			 = current_time;
			frame_info::s_main_thread_time_milli.store(static_cast<double>(delta_micro) * 0.001);

			process::pump_os_messages();

			const uint32	event_count	 = _main_window.get_event_count();
			bitmask<uint8>& window_flags = _main_window.get_flags();
			window_event*	events		 = _main_window.get_events();

			if (window_flags.is_set(window_flags::wf_close_requested))
			{
				_should_close.store(1);
				break;
			}

			for (uint32 i = 0; i < event_count; i++)
			{
				const window_event& ev = events[i];
				on_window_event(ev);
			}

			_main_window.clear_events();

			if (window_flags.is_set(window_flags::wf_size_dirty))
			{
				window_flags.remove(window_flags::wf_size_dirty);
				join_render();
				_renderer.on_window_resize(_main_window.get_size());
				kick_off_render();
			}

			/* add any fast tick events here */

			constexpr uint32 MAX_TICKS = 4;
			uint32			 ticks	   = 0;

			while (accumulator > FIXED_INTERVAL_US && ticks < MAX_TICKS)
			{
				accumulator -= FIXED_INTERVAL_US;
				tick_game_state(GAME_DT);
				ticks++;
			}

			const double interpolation = static_cast<double>(accumulator) / static_cast<double>(FIXED_INTERVAL_US);
			_renderer.populate_render_data(_update_render_frame_index);
			_current_render_frame_index.store(_update_render_frame_index, std::memory_order_release);
			_update_render_frame_index = (_update_render_frame_index + 1) % 2;
			_frame_available_semaphore.release();
		}
	}

	void game_app::tick_game_state(float delta_time)
	{
	}

	void game_app::on_window_event(const window_event& ev)
	{
		if (_renderer.on_window_event(ev))
			return;
	}

	void game_app::join_render()
	{
		REGISTER_THREAD_MAIN();

		if (_render_joined.load() == 1)
			return;

		_render_joined.store(1, std::memory_order_release);
		_frame_available_semaphore.release();

		if (_render_thread.joinable())
			_render_thread.join();

		_renderer.wait_backend();
	}

	void game_app::kick_off_render()
	{
		if (_render_joined.load() == 0)
			return;

		_render_joined.store(0, std::memory_order_release);
		_renderer.reset_render_data(0);
		_renderer.reset_render_data(1);
		_current_render_frame_index.store(1);
		_update_render_frame_index = 1;
		_render_thread			   = std::thread(&game_app::render_loop, this);
	}

	void game_app::render_loop()
	{
		const vector2ui16& screen_size = _main_window.get_size();
		REGISTER_THREAD_RENDER();

		int64 previous_time = time::get_cpu_microseconds();

		while (_render_joined.load(std::memory_order_acquire) == 0)
		{
			_frame_available_semaphore.acquire();
			const uint8 index = _current_render_frame_index.load(std::memory_order_acquire);

			const int64 current_time = time::get_cpu_microseconds();
			const int64 delta_micro	 = current_time - previous_time;
			previous_time			 = current_time;

			int64 time_before_present = 0;
			int64 time_after_present  = 0;
			_renderer.render(index, screen_size, time_before_present, time_after_present);

			const int64 present_time = time_after_present - time_before_present;
			frame_info::s_present_time_milli.store(static_cast<double>(present_time) * 0.001);

			frame_info::s_render_thread_time_milli.store(static_cast<double>(delta_micro - present_time) * 0.001);
			frame_info::s_fps.store(1.0f / static_cast<float>(delta_micro * 1e-6));
		}
	}

}
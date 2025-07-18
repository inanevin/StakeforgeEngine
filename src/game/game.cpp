// Copyright (c) 2025 Inan Evin
#include "game.hpp"
#include "platform/time.hpp"
#include "platform/process.hpp"
#include "gfx/gfx_util.hpp"
#include "common/linagx_incl.hpp"

namespace Game
{
#define GAME_DT			  1.0f / 60.0f
#define FIXED_INTERVAL_US static_cast<int64>(static_cast<double>(GAME_DT) * 1000000.0)

	void game_app::init()
	{
		time::init();
		gfx_util::init();

		_main_window.create("Game", window_flags::wf_style_windowed | window_flags::wf_high_freq, vector2i(110, 110), vector2ui(800, 600));
		_renderer.init(_main_window);
	}

	void game_app::uninit()
	{
		_renderer.uninit();
		_main_window.destroy();
		time::uninit();
		gfx_util::uninit();
	}

	void game_app::tick()
	{
		/* OS fast as possible */
		process::pump_os_messages();

		const uint32	event_count	 = _main_window.get_event_count();
		bitmask<uint8>& window_flags = _main_window.get_flags();
		window_event*	events		 = _main_window.get_events();

		if (window_flags.is_set(window_flags::wf_close_requested))
		{
			_should_exit = true;
			return;
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
			_renderer.on_window_resize(_main_window.get_size());
		}

		constexpr uint32 MAX_TICKS	 = 4;
		uint32			 ticks		 = 0;
		static int64	 previous	 = time::get_cpu_microseconds();
		static int64	 accumulator = FIXED_INTERVAL_US;
		const int64		 now		 = time::get_cpu_microseconds();
		const int64		 dt_micro	 = now - previous;
		accumulator += dt_micro;

		while (accumulator > FIXED_INTERVAL_US && ticks < MAX_TICKS)
		{
			accumulator -= FIXED_INTERVAL_US;
			tick_game_state(GAME_DT);
			_renderer.tick(GAME_DT);
			ticks++;
		}

		const double interpolation = static_cast<double>(accumulator) / static_cast<double>(FIXED_INTERVAL_US);
		_renderer.render(static_cast<float>(interpolation), _main_window.get_size());
	}

	void game_app::tick_game_state(float delta_time)
	{
	}

	void game_app::on_window_event(const window_event& ev)
	{
	}

}
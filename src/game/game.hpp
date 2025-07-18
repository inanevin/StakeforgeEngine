// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "gfx/renderer.hpp"
#include "platform/window.hpp"

namespace Game
{
	class window;
	struct window_event;

	class game_app
	{
	public:
		void init();
		void uninit();
		void tick();
		void tick_game_state(float delta_time);
		void on_window_event(const window_event& ev);

		inline bool get_should_exit() const
		{
			return _should_exit;
		}

		inline window& get_main_window()
		{
			return _main_window;
		}

	private:
		window	 _main_window = {};
		renderer _renderer	  = {};
		bool	 _should_exit = false;
	};
}
// Copyright (c) 2025 Inan Evin
#include "game.hpp"
#include "platform/window.hpp"
#include "gfx/renderer.hpp"
#include "platform/time.hpp"
#include "platform/process.hpp"
#include "memory/memory_tracer.hpp"
#include "common/system_info.hpp"
#include "gfx/common/render_data.hpp"
#include "gfx/backend/backend.hpp"
#include "io/log.hpp"
#include "debug_console.hpp"
#include "project/engine_data.hpp"
#include "world/world.hpp"
#include "gfx/world/world_renderer.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#endif

namespace SFG
{

#define SFG_DT 1.0f / 60.0f

	static uint8 wr = 0;

	void game_app::init(const vector2ui16& render_target_size)
	{

		SET_INIT(true);
		REGISTER_THREAD_MAIN();

		time::init();
		debug_console::init();

		_world = new world();

		_main_window = new window();
		_main_window->create("Game", window_flags::wf_style_windowed, vector2i16(0, 0), render_target_size);
		_window_size = render_target_size;

		// vector<monitor_info> out_infos;
		// process::get_all_monitors(out_infos);
		// _main_window.set_position(out_infos[1].position);
		// _main_window.set_size(out_infos[1].work_size);

		gfx_backend::s_instance = new gfx_backend();
		gfx_backend* backend	= gfx_backend::get();
		backend->init();

		_renderer = new renderer();
		_renderer->init(_main_window, _world);

		engine_data::get().init();

#ifdef SFG_TOOLMODE
		const string& last_world = engine_data::get().get_last_world();
		if (file_system::exists(last_world.c_str()))
			_world->load(last_world.c_str());
		else
		{
			engine_data::get().set_last_world("");
			_world->load("");
		}
#endif

		/*************** DEBUG *************/
		_world->load_debug();
		/*************** DEBUG *************/

		_render_joined.store(1);
		kick_off_render();

		/*************** CONSOLE *************/
		debug_console::get()->register_console_function("app_new_world", [this]() {
			join_render();
			_world->load("");
			kick_off_render();
		});

		debug_console::get()->register_console_function<const char*>("app_save_world", [this](const char* path) {
			const string p = engine_data::get().get_working_dir() + path;
			_world->save(p.c_str());
			engine_data::get().set_last_world(p);
			engine_data::get().save();
		});

		debug_console::get()->register_console_function<const char*>("app_load_world", [this](const char* path) {
			join_render();
			const string p = engine_data::get().get_working_dir() + path;
			_world->load(p.c_str());
			kick_off_render();

			engine_data::get().set_last_world(p);
			engine_data::get().save();
		});

		SET_INIT(false);
	}

	void game_app::uninit()
	{
		frame_info::s_is_render_active = false;
		_world->uninit();
		delete _world;

		engine_data::get().uninit();

		join_render();

		time::uninit();
		debug_console::uninit();

		_renderer->uninit();
		delete _renderer;

		_main_window->destroy();
		delete _main_window;

		gfx_backend* backend = gfx_backend::get();
		backend->uninit();
		delete gfx_backend::s_instance;
		gfx_backend::s_instance = nullptr;
	}

	void game_app::tick()
	{
		const int64 FIXED_INTERVAL_US = (int64)1000000 / (int64)SFG_DT;
		int64		previous_time	  = time::get_cpu_microseconds();
		int64		accumulator		  = FIXED_INTERVAL_US;

		while (_should_close.load(std::memory_order_acquire) == 0)
		{
			const int64 current_time = time::get_cpu_microseconds();
			const int64 delta_micro	 = current_time - previous_time;
			previous_time			 = current_time;
			frame_info::s_main_thread_time_milli.store(static_cast<double>(delta_micro) * 0.001);

			process::pump_os_messages();

			const uint32	  event_count  = _main_window->get_event_count();
			bitmask<uint8>&	  window_flags = _main_window->get_flags();
			window_event*	  events	   = _main_window->get_events();
			const vector2ui16 ws		   = _main_window->get_size();

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

			_main_window->clear_events();

			if (window_flags.is_set(window_flags::wf_size_dirty))
			{
				window_flags.remove(window_flags::wf_size_dirty);
				join_render();
				_renderer->on_window_resize(ws);
				_window_size = ws;
				kick_off_render();
			}
			/* add any fast tick events here */

			constexpr uint32 MAX_TICKS = 4;
			uint32			 ticks	   = 0;

			accumulator += delta_micro * 1000;
			while (accumulator >= FIXED_INTERVAL_US && ticks < MAX_TICKS)
			{
				accumulator -= FIXED_INTERVAL_US;
				_world->tick(_update_render_frame_index, ws, SFG_DT);
				ticks++;
			}

			const double interpolation = static_cast<double>(accumulator) / static_cast<double>(FIXED_INTERVAL_US);
			_renderer->populate_render_data(_update_render_frame_index, interpolation);
			_current_render_frame_index.store(_update_render_frame_index, std::memory_order_release);
			_update_render_frame_index = (_update_render_frame_index + 1) % 2;
			_frame_available_semaphore.release();
			frame_info::s_frame.fetch_add(1);
		}
	}

	void game_app::on_window_event(const window_event& ev)
	{
		if (_renderer->on_window_event(ev))
			return;

		_world->on_window_event(ev);
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

		frame_info::s_is_render_active = false;
		_renderer->wait_backend();
	}

	/*
	void game_app::pipe_read()
	{
		char  buffer[PIPE_MAX_MSG_SIZE];
		DWORD bytesRead;

		const HANDLE pipe = static_cast<HANDLE>(process::get_pipe_handle());

		// Read loop
		while (pipe != INVALID_HANDLE_VALUE)
		{
			if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
			{
				if (bytesRead > 0)
				{

					buffer[bytesRead] = '\0'; // Null-terminate the string
					istream		   stream((uint8*)buffer, static_cast<size_t>(bytesRead));
					pipe_data_type data_type = {};
					stream >> data_type;

					if (data_type == pipe_data_type::log)
					{
						log_level lvl = {};
						stream >> lvl;

						const char* msg = reinterpret_cast<const char*>(stream.get_data_current());
						log::instance().log_msg(lvl, msg);
					}
				}
			}
			else
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_BROKEN_PIPE)
				{
					SFG_ERR("pipe_read() -> pipe disconnected! {0}", dwError);
					break;
				}
			}
		}
	}
*/

	void game_app::kick_off_render()
	{
		if (_render_joined.load() == 0)
			return;

		_render_joined.store(0, std::memory_order_release);
		_current_render_frame_index.store(1);
		_update_render_frame_index	   = 1;
		_render_thread				   = std::thread(&game_app::render_loop, this);
		frame_info::s_is_render_active = true;
	}

	void game_app::render_loop()
	{
		const vector2ui16& screen_size = _window_size;
		REGISTER_THREAD_RENDER();

		int64 previous_time = time::get_cpu_microseconds();

		while (_render_joined.load(std::memory_order_acquire) == 0)
		{
			_frame_available_semaphore.acquire();
			const uint8 index = _current_render_frame_index.load(std::memory_order_acquire);

#ifndef SFG_PRODUCTION
			const int64 current_time = time::get_cpu_microseconds();
			const int64 delta_micro	 = current_time - previous_time;
			previous_time			 = current_time;
#endif

			_world->pre_render(index, screen_size);
			_renderer->render(index, screen_size);
			frame_info::s_render_frame.fetch_add(1);

#ifndef SFG_PRODUCTION
			frame_info::s_present_time_milli.store(frame_info::get_present_time_micro() * 0.001);
			frame_info::s_render_thread_time_milli.store(static_cast<double>(delta_micro - frame_info::s_present_time_micro) * 0.001);
			frame_info::s_fps.store(1.0f / static_cast<float>(delta_micro * 1e-6));
#endif
		}
	}
}
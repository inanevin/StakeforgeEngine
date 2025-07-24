// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "gfx/common/render_data.hpp"
#include "gfx/buffer_queue.hpp"
#include "gfx/texture_queue.hpp"
#include "memory/bump_allocator.hpp"
#include "app/debug_controller.hpp"

namespace Game
{
	class vector2ui;
	class window;
	class render_data;
	struct window_event;

	class renderer
	{
	public:
		void init(const window& main_window);
		void uninit();
		void wait_backend();
		void populate_render_data(uint8 index);
		void render(uint8 index, const vector2ui& size);
		bool on_window_event(const window_event& ev);
		void on_window_resize(const vector2ui& size);
		void reset_render_data(uint8 index);

	private:
		void send_uploads();

	private:
		struct per_frame_data
		{
			semaphore_data sem_frame = {};
			semaphore_data sem_copy	 = {};
			resource_id	   cmd_gfx	 = 0;
			resource_id	   cmd_copy	 = 0;
		};

	private:
		debug_controller _debug_controller = {};
		per_frame_data	 _pfd[FRAMES_IN_FLIGHT];
		bump_allocator	 _frame_allocator[FRAMES_IN_FLIGHT] = {};
		buffer_queue	 _buffer_queue						= {};
		texture_queue	 _texture_queue						= {};
		render_data		 _render_data[2];
		resource_id		 _swapchain_main = 0;
		uint8			 _frame_index	 = 0;
	};
}
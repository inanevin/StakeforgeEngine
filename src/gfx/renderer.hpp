// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "gfx/common/render_data.hpp"
#include "memory/bump_allocator.hpp"
#include "resources/shader.hpp"

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
		struct per_frame_data
		{
			semaphore_data frame_semaphore = {};
			resource_id	   gfx_buffer	   = 0;
		};

	private:
		per_frame_data _pfd[FRAMES_IN_FLIGHT];
		bump_allocator _frame_allocator[FRAMES_IN_FLIGHT] = {};
		shader		   _shader_gui_default				  = {};
		render_data	   _render_data[2];
		resource_id	   _swapchain_main			= 0;
		resource_id	   _bind_layout_gui_default = 0;
		uint8		   _frame_index				= 0;
	};
}
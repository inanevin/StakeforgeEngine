// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "gfx/common/render_data.hpp"
#include "gfx/common/barrier_description.hpp"
#include "gfx/buffer.hpp"
#include "gfx/buffer_queue.hpp"
#include "gfx/texture_queue.hpp"
#include "resources/shader.hpp"
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
		void render(uint8 index, const vector2ui16& size);
		bool on_window_event(const window_event& ev);
		void on_window_resize(const vector2ui16& size);
		void reset_render_data(uint8 index);

	private:
		void send_uploads(uint8 frame_index);
		void send_barriers(resource_id cmd_list);

	private:
		struct buf_engine_global
		{
			float f1 = 0.0f;
			float f2 = 0.0;
		};

		struct per_frame_data
		{
			buffer		   buf_engine_global	= {};
			semaphore_data sem_frame			= {};
			semaphore_data sem_copy				= {};
			resource_id	   cmd_gfx				= 0;
			resource_id	   cmd_copy				= 0;
			resource_id	   bind_group_global	= 0;
			resource_id	   bind_group_swapchain = 0;
		};

		struct gfx_data
		{
			resource_id swapchain_main	   = 0;
			resource_id bind_layout_global = 0;
			resource_id dummy_ubo		   = 0;
			resource_id dummy_ssbo		   = 0;
			resource_id dummy_sampler	   = 0;
			resource_id dummy_texture	   = 0;
			uint8		frame_index		   = 0;
		};

		struct shader_data
		{
			shader swapchain = {};
		};

	private:
		debug_controller _debug_controller = {};
		gfx_data		 _gfx_data		   = {};
		shader_data		 _shaders		   = {};
		per_frame_data	 _pfd[FRAMES_IN_FLIGHT];
		bump_allocator	 _frame_allocator[FRAMES_IN_FLIGHT] = {};
		buffer_queue	 _buffer_queue						= {};
		texture_queue	 _texture_queue						= {};
		render_data		 _render_data[2];
		vector<barrier>	 _reuse_barriers;
	};
}
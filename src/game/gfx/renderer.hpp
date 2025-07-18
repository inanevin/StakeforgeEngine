// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "mesh_manager.hpp"
#include "gfx/gfx_util.hpp"
#include "gfx/render_pass.hpp"
#include "gfx/resource_queue.hpp"
#include "gfx/buffer.hpp"
#include "gfx/gfx_semaphore.hpp"
#include "data/vector.hpp"
#include "gui/vekt.hpp"
#include "resources/shader.hpp"
#include "resources/material.hpp"
#include "resources/texture_sampler.hpp"
#include "math/matrix4x4.hpp"
#include "math/vector4.hpp"

namespace LinaGX
{
	class CommandStream;
}

namespace vekt
{
	class atlas;
	struct font;
}

namespace Game
{
	class window;

	class renderer
	{

	private:
		struct per_frame_data
		{
			LinaGX::CommandStream* stream_gfx				= nullptr;
			LinaGX::CommandStream* stream_copy				= nullptr;
			gfx_semaphore		   semaphore_copy			= {};
			uint16				   descriptor_set_global	= 0;
			uint16				   descriptor_set_debug_gui = 0;
			buffer				   buffer_global_data		= {};
			buffer				   buffer_gui_ubo			= {};
			buffer				   buffer_gui_vertex		= {};
			buffer				   buffer_gui_index			= {};
			uint32				   vertex_counter_gui		= 0;
			uint32				   index_counter_gui		= 0;
		};

		struct atlas_ref
		{
			vekt::atlas* atlas = nullptr;
			material	 mat   = {};
			uint32		 hw	   = 0;
		};

		struct global_data
		{
			vector4 delta_elapsed;
		};

		struct gui_data
		{
			matrix4x4 projection;
		};

		struct gui_material_sdf
		{
			float thickness = 0.0f;
			float softness	= 0.0f;
		};

	public:
		void init(window& main_window);
		void uninit();

		void tick(float dt);
		void join();
		void render(float alpha, const vector2ui& size);
		void on_window_resize(const vector2ui& size);

	private:
		void atlas_created(vekt::atlas* atlas);
		void atlas_updated(vekt::atlas* atlas);
		void atlas_destroyed(vekt::atlas* atlas);
		void on_gui_draw(const vekt::draw_buffer& buffer);
		void flush_copy_queue(per_frame_data& pfd);

	private:
		vector<atlas_ref> _atlas_refs = {};

		resource_queue _resource_queue;
		per_frame_data _pfd[gfx_util::FRAMES_IN_FLIGHT];
		mesh_manager   _mesh_manager   = {};
		render_pass	   _pass_debug_gui = {};

		vekt::builder _vekt_builder = {};

		vekt::font*		_font_debug				   = nullptr;
		uint32			_txt_world				   = 0;
		uint16			_pipeline_layout_global	   = 0;
		uint16			_pipeline_layout_debug_gui = 0;
		uint8			_swapchain				   = 0;
		uint32			_frame_index			   = 0;
		shader			_shader_gui_default		   = {};
		shader			_shader_gui_text		   = {};
		shader			_shader_gui_sdf			   = {};
		texture_sampler _sampler_gui_text		   = {};
	};
}
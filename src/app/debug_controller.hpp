// Copyright (c) 2025 Inan Evin
#pragma once
#include "resources/shader.hpp"
#include "math/matrix4x4.hpp"
#include "math/vector4ui16.hpp"
#include "data/vector.hpp"
#include "gfx/buffer.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "memory/text_allocator.hpp"
#include "data/string.hpp"
#include "vendor/moodycamel/readerwriterqueue.h"

namespace vekt
{
	class builder;
	struct draw_buffer;
	class atlas;
	struct font;
}

namespace Game
{
	enum class log_level;
	class vector2ui;
	class render_pass;
	class bump_allocator;
	class buffer_queue;
	class texture_queue;
	struct window_event;
	struct barrier;

#define MAX_GUI_DRAW_CALLS 32
#define MAX_KEY_EVENTS	   64

	class debug_controller
	{
	public:
		void init(texture_queue* texture_queue, resource_id global_bind_layout, const vector2ui16& screen_size);
		void uninit();
		void collect_barriers(vector<barrier>& out_barriers);
		void render(resource_id cmd_buffer, uint8 frame_index, bump_allocator& alloc);
		void upload(buffer_queue& q, uint8 frame_index);
		void on_window_resize(const vector2ui16& size);
		bool on_window_event(const window_event& ev);

		inline resource_id get_final_rt(uint8 frame_index) const
		{
			return _pfd[frame_index].rt_fullscreen;
		}

	private:
		void on_log(log_level lvl, const char* msg);

		void flush_key_events();
		void build_console();
		void console_logic();
		void add_console_text(const char* text, log_level level);
		void update_console_input_field();
		void on_draw(const vekt::draw_buffer& buffer);
		void on_atlas_created(vekt::atlas* atlas);
		void on_atlas_updated(vekt::atlas* atlas);
		void on_atlas_destroyed(vekt::atlas* atlas);
		void set_console_visible(bool visible);

	private:
		enum class console_state : uint8
		{
			invisible = 0,
			visible,
		};

		struct input_event
		{
			uint16 button = 0;
			int16  wheel  = 0;
		};

		struct gui_draw_call
		{
			vector4ui16 scissors	= vector4ui16::zero;
			uint16		start_vtx	= 0;
			uint16		start_idx	= 0;
			uint16		index_count = 0;
			resource_id shader		= 0;
			resource_id bind_group	= 0;
		};

		struct per_frame_data
		{
			buffer		 buf_gui_vtx				= {};
			buffer		 buf_gui_idx				= {};
			buffer		 buf_gui_pass_view			= {};
			buffer		 buf_fullscreen_pass_view	= {};
			resource_id	 bind_group_gui_render_pass = 0;
			resource_id	 bind_group_fullscreen		= 0;
			resource_id	 rt_console					= 0;
			resource_id	 rt_fullscreen				= 0;
			unsigned int counter_vtx				= 0;
			unsigned int counter_idx				= 0;
			uint16		 draw_call_count			= 0;

			inline void reset()
			{
				counter_vtx = counter_idx = 0;
				draw_call_count			  = 0;
			}
		};

		struct gui_pass_view
		{
			matrix4x4 proj			= matrix4x4::identity;
			float	  sdf_thickness = 0.5f;
			float	  sdf_softness	= 0.02f;
		};

		struct fullscreen_pass_view
		{
			vector2 size = vector2::zero;
		};

		struct atlas_ref
		{
			vekt::atlas*   atlas			   = nullptr;
			resource_id	   texture			   = 0;
			resource_id	   bind_group		   = 0;
			resource_id	   intermediate_buffer = 0;
			texture_buffer buffer			   = {};
			uint8		   res_alive		   = false;
		};

		struct shaders
		{
			shader gui_default					 = {};
			shader gui_text						 = {};
			shader gui_sdf						 = {};
			shader debug_controller_console_draw = {};
		};

		struct gfx_data
		{
			vector<atlas_ref> atlases;
			texture_queue*	  texture_queue = nullptr;
			vector2ui16		  window_size	= vector2ui16::zero;
			vector2ui16		  rt_size		= vector2ui16::zero;
			uint64			  frame_counter = 0;
			uint8			  frame_index	= 0;
		};

		struct vekt_data
		{
			vector<int32>  console_texts			 = {};
			vekt::builder* builder					 = nullptr;
			vekt::font*	   font_debug				 = nullptr;
			vekt::font*	   font_icon				 = nullptr;
			int32		   widget_console_bg		 = -1;
			int32		   widget_input_field		 = -1;
			int32		   widget_input_text		 = {};
			int32		   widget_border			 = {};
			int32		   widget_fps				 = 0;
			int32		   widget_main_thread		 = 0;
			int32		   widget_render_thread		 = 0;
			int32		   widget_present_time		 = 0;
			int32		   widget_global_mem		 = 0;
			int32		   widget_gfx_mem			 = 0;
			float		   console_total_text_size_y = 0.0f;
		};

		struct input_field
		{
			vector<const char*> history			  = {};
			const char*			text			  = nullptr;
			int16				scroll_amt		  = 0;
			int8				history_traversal = 0;
			int8				caret_pos		  = 0;
			int8				text_size		  = 0;
		};

	private:
		text_allocator<10000>									   _text_allocator = {};
		shaders													   _shaders		   = {};
		gfx_data												   _gfx_data	   = {};
		vekt_data												   _vekt_data	   = {};
		input_field												   _input_field	   = {};
		per_frame_data											   _pfd[FRAMES_IN_FLIGHT];
		gui_draw_call											   _gui_draw_calls[MAX_GUI_DRAW_CALLS];
		moodycamel::ReaderWriterQueue<input_event, MAX_KEY_EVENTS> _input_events;
		console_state											   _console_state = console_state::invisible;
	};
}
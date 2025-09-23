// Copyright (c) 2025 Inan Evin

#include "debug_controller.hpp"
#include "math/vector2ui.hpp"
#include "data/vector_util.hpp"
#include "memory/memory.hpp"
#include "memory/bump_allocator.hpp"
#include "gfx/buffer_queue.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/util/gfx_util.hpp"
#include "gfx/texture_queue.hpp"
#include "common/system_info.hpp"
#include "platform/window_common.hpp"
#include "io/log.hpp"
#include "math/math.hpp"
#include "math/color.hpp"
#include "platform/process.hpp"
#include "input/input_mappings.hpp"
#include "debug_console.hpp"
#include "memory/memory_tracer.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"
#include "serialization/serialization.hpp"
#include "io/file_system.hpp"

#define VEKT_STRING_CSTR
#define VEKT_VEC4 SFG::vector4
#define VEKT_VEC2 SFG::vector2
#define VEKT_IMPL
#include "gui/vekt.hpp"

namespace SFG
{

#define MAX_CONSOLE_TEXT		128
#define MAX_INPUT_FIELD			127
#define COLOR_TEXT				color::srgb_to_linear(color(129.0f / 255.0f, 220.0f / 255.0f, 148.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_TEXT_WARN			color::srgb_to_linear(color(240.0f / 255.0f, 220.0f / 255.0f, 148.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_TEXT_PROGRESS		color::srgb_to_linear(color(148.0f / 255.0f, 170.0f / 255.0f, 240.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_TEXT_ERR			color::srgb_to_linear(color(250.0f / 255.0f, 120.0f / 255.0f, 88.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_TEXT_DARK			color::srgb_to_linear(color(119.0f / 255.0f, 210.0f / 255.0f, 138.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_CONSOLE_BG		color::srgb_to_linear(color(12.0f / 255.0f, 16.0f / 255.0f, 12.0f / 255.0f, 0.99f)).to_vector()
#define COLOR_CONSOLE_BG_OPAQUE color::srgb_to_linear(color(12.0f / 255.0f, 16.0f / 255.0f, 12.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_BORDER			color::srgb_to_linear(color(89.0f / 255.0f, 180.0f / 255.0f, 108.0f / 255.0f, 1.0f)).to_vector()
#define DEBUG_FONT_SIZE			20
#define INPUT_FIELD_HEIGHT		static_cast<float>(DEBUG_FONT_SIZE) * 1.5f
#define CONSOLE_SPACING			static_cast<float>(DEBUG_FONT_SIZE) * 0.5f
#define MAX_HISTORY				8
#define RT_FORMAT				format::r8g8b8a8_srgb
#define HISTORY_PATH			"console_history.stk"

	static constexpr float B_TO_MB = 1024.0f * 1024.0f;

	void debug_controller::build_console()
	{
		_input_field.text = _text_allocator.allocate(MAX_INPUT_FIELD);

		// vekt root
		{
			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(_vekt_data.builder->get_root());
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_column;
		}

		// header
		{
			vekt::id header = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.builder->get_root(), header);

			_vekt_data.builder->widget_set_pos(header, vector2(0.0f, 0.0f));
			_vekt_data.builder->widget_set_size(header, vector2(1.0f, INPUT_FIELD_HEIGHT), vekt::helper_size_type::relative, vekt::helper_size_type::absolute);

			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(header);
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_row;

			vekt::size_props& props	 = _vekt_data.builder->widget_get_size_props(header);
			props.child_margins.left = static_cast<float>(DEBUG_FONT_SIZE) * 0.5f;
			props.spacing			 = CONSOLE_SPACING;

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(header);
			gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
			gfx.color			  = COLOR_TEXT;

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("FPS: 1000");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_fps = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("Main Thread (ms): 0.000000 ");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_main_thread = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("Render Thread (ms): 0.000000 ");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_render_thread = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("Present (ms): 0.000000 ");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_present_time = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("Glob Memory (mb): 0.00000");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_global_mem = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}

			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_text;
				gfx.color			  = COLOR_CONSOLE_BG_OPAQUE;

				vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
				tp.font				 = _vekt_data.font_debug;
				tp.text				 = _text_allocator.allocate("Gfx Memory (mb): 0.00000");
				_vekt_data.builder->widget_update_text(w);

				_vekt_data.widget_gfx_mem = w;
			}

			// border
			{
				vekt::id w = _vekt_data.builder->allocate();
				_vekt_data.builder->widget_add_child(header, w);
				_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);
				_vekt_data.builder->widget_set_size(w, vector2(DEBUG_FONT_SIZE * 0.2f, 1.0f), vekt::helper_size_type::absolute, vekt::helper_size_type::relative);
				vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
				gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
				gfx.color			  = COLOR_CONSOLE_BG;
			}
		}

		// Console parent
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.builder->get_root(), w);
			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f), vekt::helper_pos_type::relative, vekt::helper_pos_type::absolute);
			_vekt_data.builder->widget_set_size(w, vector2(1.0f, 1.0f), vekt::helper_size_type::relative, vekt::helper_size_type::fill);

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_rect | vekt::gfx_flags::gfx_clip_children;
			gfx.color			  = COLOR_CONSOLE_BG;

			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(w);
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_column;

			vekt::size_props& props	   = _vekt_data.builder->widget_get_size_props(w);
			props.child_margins.left   = CONSOLE_SPACING;
			props.child_margins.top	   = CONSOLE_SPACING;
			props.child_margins.bottom = CONSOLE_SPACING;
			props.spacing			   = CONSOLE_SPACING;

			_vekt_data.widget_console_bg = w;
		}

		// border
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.builder->get_root(), w);
			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f), vekt::helper_pos_type::relative, vekt::helper_pos_type::absolute);
			_vekt_data.builder->widget_set_size(w, vector2(1.0f, DEBUG_FONT_SIZE * 0.05f), vekt::helper_size_type::relative, vekt::helper_size_type::absolute);
			vekt::widget_gfx& gfx	 = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags				 = vekt::gfx_flags::gfx_is_rect;
			gfx.color				 = COLOR_BORDER;
			_vekt_data.widget_border = w;
		}

		// Input Field
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.builder->get_root(), w);

			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f));
			_vekt_data.builder->widget_set_size(w, vector2(1.0f, INPUT_FIELD_HEIGHT), vekt::helper_size_type::relative, vekt::helper_size_type::absolute);

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
			gfx.color			  = COLOR_CONSOLE_BG;

			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(w);
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_row;

			vekt::size_props& props		  = _vekt_data.builder->widget_get_size_props(w);
			props.child_margins.left	  = static_cast<float>(DEBUG_FONT_SIZE) * 0.5f;
			props.spacing				  = CONSOLE_SPACING;
			_vekt_data.widget_input_field = w;
		}

		// icon
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.widget_input_field, w);
			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_text;
			gfx.color			  = COLOR_TEXT;

			vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
			tp.font				 = _vekt_data.font_icon;
			tp.text				 = _text_allocator.allocate("\u0071");
			_vekt_data.builder->widget_update_text(w);
		}

		// input text
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.widget_input_field, w);
			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.5f), vekt::helper_pos_type::relative, vekt::helper_pos_type::relative, vekt::helper_anchor_type::start, vekt::helper_anchor_type::center);

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_text;
			gfx.color			  = COLOR_TEXT;

			vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
			tp.font				 = _vekt_data.font_debug;
			tp.text				 = _input_field.text;

			_vekt_data.widget_input_text = w;
			_vekt_data.builder->widget_update_text(w);
		}
		set_console_visible(false);
	}

	void debug_controller::init(texture_queue* texture_queue, gfx_id global_bind_layout, const vector2ui16& screen_size)
	{

		_gfx_data.texture_queue = texture_queue;
		_gfx_data.rt_size		= vector2ui16(screen_size.x, screen_size.y / 2);
		_gfx_data.window_size	= vector2ui16(screen_size.x, screen_size.y);

		gfx_backend* backend = gfx_backend::get();

		// gui default
		_shaders.gui_default.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_default.stkfrg", false, global_bind_layout);

		// gui text
		_shaders.gui_text.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_text.stkfrg", false, global_bind_layout);

		// gui sdf
		_shaders.gui_sdf.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_sdf.stkfrg", false, global_bind_layout);

		// console draw
		_shaders.debug_controller_console_draw.create_from_file_vertex_pixel("assets/engine/shaders/debug_controller/console_draw.stkfrg", false, global_bind_layout);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			pfd.rt_console = backend->create_texture({
				.texture_format = RT_FORMAT,
				.size			= vector2ui16(screen_size.x, screen_size.y / 2),
				.flags			= texture_flags::tf_sampled | texture_flags::tf_is_2d | texture_flags::tf_render_target,
				.clear_values	= {0.0f, 0.0f, 0.0f, 0.0f},
				.debug_name		= "console_rt",
			});

			pfd.rt_fullscreen = backend->create_texture({
				.texture_format = RT_FORMAT,
				.size			= vector2ui16(screen_size.x, screen_size.y),
				.flags			= texture_flags::tf_sampled | texture_flags::tf_is_2d | texture_flags::tf_render_target,
				.clear_values	= {0.0f, 0.0f, 0.0f, 0.0f},
				.debug_name		= "debug_rt",
			});

			pfd.buf_gui_pass_view.create_hw({
				.size		= sizeof(gui_pass_view),
				.flags		= resource_flags::rf_cpu_visible | resource_flags::rf_constant_buffer,
				.debug_name = "cbv_gui_pass",
			});

			pfd.buf_fullscreen_pass_view.create_hw({
				.size		= sizeof(fullscreen_pass_view),
				.flags		= resource_flags::rf_cpu_visible | resource_flags::rf_constant_buffer,
				.debug_name = "cbv_fs_pass",
			});

			pfd.bind_group_gui_render_pass = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group_gui_render_pass, rpi_table_render_pass, 1, false);
			backend->bind_group_update_pointer(pfd.bind_group_gui_render_pass,
											   0,
											   {
												   {.resource = pfd.buf_gui_pass_view.get_hw_gpu(), .pointer_index = upi_render_pass_ubo0, .type = binding_type::ubo},
											   });

			pfd.bind_group_fullscreen = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group_fullscreen, rpi_table_material, 3, false);
			backend->bind_group_update_pointer(pfd.bind_group_fullscreen,
											   0,
											   {
												   {.resource = pfd.buf_fullscreen_pass_view.get_hw_gpu(), .pointer_index = upi_material_ubo0, .type = binding_type::ubo},
												   {.resource = pfd.rt_console, .pointer_index = upi_material_texture0, .type = binding_type::texture_binding},
											   });

			pfd.buf_gui_vtx.create_staging_hw(
				{
					.size		= sizeof(vekt::vertex) * 14000,
					.flags		= resource_flags::rf_vertex_buffer | resource_flags::rf_cpu_visible,
					.debug_name = "gui_vertex_stg",
				},
				{
					.size		= sizeof(vekt::vertex) * 14000,
					.flags		= resource_flags::rf_vertex_buffer | resource_flags::rf_gpu_only,
					.debug_name = "gui_vertex_gpu",
				});

			pfd.buf_gui_idx.create_staging_hw(
				{
					.size		= sizeof(vekt::index) * 24000,
					.flags		= resource_flags::rf_index_buffer | resource_flags::rf_cpu_visible,
					.debug_name = "gui_index_stg",
				},
				{
					.size		= sizeof(vekt::index) * 24000,
					.flags		= resource_flags::rf_index_buffer | resource_flags::rf_gpu_only,
					.debug_name = "gui_index_gpu",
				});
		}

		_vekt_data.builder = new vekt::builder();
		_vekt_data.builder->init({
			.vertex_buffer_sz			 = 1024 * 1024 * 10,
			.index_buffer_sz			 = 1024 * 1024 * 20,
			.text_cache_vertex_buffer_sz = 1024 * 1024 * 10,
			.text_cache_index_buffer_sz	 = 1024 * 1024 * 20,
			.buffer_count				 = 5,
		});

		_vekt_data.builder->set_on_draw([this](const vekt::draw_buffer& buffer) { on_draw(buffer); });
		vekt::font_manager& font_manager = vekt::font_manager::get();

		font_manager.init();
		font_manager.set_atlas_created_callback(std::bind(&debug_controller::on_atlas_created, this, std::placeholders::_1));
		font_manager.set_atlas_updated_callback(std::bind(&debug_controller::on_atlas_updated, this, std::placeholders::_1));
		font_manager.set_atlas_destroyed_callback(std::bind(&debug_controller::on_atlas_destroyed, this, std::placeholders::_1));
		_vekt_data.font_debug = font_manager.load_font("assets/engine/fonts/VT323-Regular.ttf", DEBUG_FONT_SIZE);
		_vekt_data.font_icon  = font_manager.load_font("assets/engine/fonts/icons.ttf", 12, 32, 128, vekt::font_type::sdf);

		_vekt_data.console_texts.reserve(MAX_CONSOLE_TEXT);
		_input_field.history.reserve(MAX_CONSOLE_TEXT);

		build_console();

		// load history
		if (file_system::exists(HISTORY_PATH))
		{
			istream in	 = serialization::load_from_file(HISTORY_PATH);
			uint8	size = 0;
			in >> _console_state;
			in >> size;

			set_console_visible(_console_state == console_state::visible);

			for (uint8 i = 0; i < size; i++)
			{
				uint8 len = 0;
				in >> len;

				const char* history = _text_allocator.allocate(static_cast<size_t>(len));
				in.read_to_raw((uint8*)history, static_cast<size_t>(len));
				_input_field.history.push_back(history);
			}

			in.destroy();
		}

		log::instance().add_listener(TO_SIDC("debug_controller"), std::bind(&debug_controller::on_log, this, std::placeholders::_1, std::placeholders::_2));
	}

	void debug_controller::uninit()
	{
		// save history
		{
			const uint8 history_sz = static_cast<uint8>(_input_field.history.size());
			ostream		out;
			out << _console_state;
			out << history_sz;

			for (const char* el : _input_field.history)
			{
				const uint8 len = static_cast<uint8>(strlen(el));
				out << len;
				out.write_raw((uint8*)el, static_cast<size_t>(len));
			}

			serialization::save_to_file(HISTORY_PATH, out);
			out.destroy();
		}

		vekt::font_manager::get().unload_font(_vekt_data.font_debug);
		vekt::font_manager::get().unload_font(_vekt_data.font_icon);
		vekt::font_manager::get().uninit();

		SFG_ASSERT(_gfx_data.atlases.empty());

		_vekt_data.builder->uninit();
		delete _vekt_data.builder;

		gfx_backend* backend = gfx_backend::get();

		_shaders.gui_default.destroy();
		_shaders.gui_text.destroy();
		_shaders.gui_sdf.destroy();
		_shaders.debug_controller_console_draw.destroy();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_texture(pfd.rt_console);
			backend->destroy_texture(pfd.rt_fullscreen);
			backend->destroy_bind_group(pfd.bind_group_gui_render_pass);
			backend->destroy_bind_group(pfd.bind_group_fullscreen);
			backend->unmap_resource(pfd.buf_gui_pass_view.get_hw_gpu());
			backend->unmap_resource(pfd.buf_fullscreen_pass_view.get_hw_gpu());
			backend->unmap_resource(pfd.buf_gui_vtx.get_hw_staging());
			backend->unmap_resource(pfd.buf_gui_idx.get_hw_staging());
			pfd.buf_gui_pass_view.destroy();
			pfd.buf_gui_vtx.destroy();
			pfd.buf_gui_idx.destroy();
			pfd.buf_fullscreen_pass_view.destroy();
		}

		log::instance().remove_listener(TO_SIDC("debug_controller"));
	}

	void debug_controller::upload(buffer_queue& q, uint8 frame_index)
	{
		_gfx_data.frame_index = frame_index;

		per_frame_data& pfd = _pfd[frame_index];
		pfd.reset();
		q.add_request({.buffer = &pfd.buf_gui_idx});
		q.add_request({.buffer = &pfd.buf_gui_vtx});

		const gui_pass_view view = {
			.proj		   = matrix4x4::ortho(0, static_cast<float>(_gfx_data.rt_size.x), 0, static_cast<float>(_gfx_data.rt_size.y), 0.0f, 1.0f),
			.sdf_thickness = 0.5f,
			.sdf_softness  = 0.02f,
		};
		pfd.buf_gui_pass_view.buffer_data(0, (void*)&view, sizeof(gui_pass_view));

		const fullscreen_pass_view fs_view = {
			.size = vector2(static_cast<float>(_gfx_data.rt_size.x), static_cast<float>(_gfx_data.rt_size.y)),
		};
		pfd.buf_fullscreen_pass_view.buffer_data(0, (void*)&fs_view, sizeof(fullscreen_pass_view));

		flush_key_events();

		_vekt_data.builder->build_begin(vector2(_gfx_data.rt_size.x, _gfx_data.rt_size.y));
		console_logic();
		_vekt_data.builder->build_end();
		_vekt_data.builder->flush();
	}

	void debug_controller::collect_barriers(vector<barrier>& out_barriers)
	{
		per_frame_data& pfd = _pfd[_gfx_data.frame_index];
		out_barriers.push_back({
			.resource	= pfd.rt_console,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::ps_resource,
			.to_state	= resource_state::render_target,
		});

		out_barriers.push_back({
			.resource	= pfd.rt_fullscreen,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::ps_resource,
			.to_state	= resource_state::render_target,
		});
	}

	void debug_controller::tick()
	{
		const char* cmd = nullptr;
		while (_commands.try_dequeue(cmd))
		{
			debug_console::get()->parse_console_command(cmd);
			SFG_FREE((void*)cmd);
		}
	}

	void debug_controller::render(gfx_id cmd_buffer, uint8 frame_index, bump_allocator& alloc)
	{
		per_frame_data&	  pfd				= _pfd[frame_index];
		const vector2ui16 rt_size			= _gfx_data.rt_size;
		const gfx_id	  rt_console		= pfd.rt_console;
		const gfx_id	  rt_fullscreen		= pfd.rt_fullscreen;
		const gfx_id	  gui_vertex		= pfd.buf_gui_vtx.get_hw_gpu();
		const gfx_id	  gui_index			= pfd.buf_gui_idx.get_hw_gpu();
		const gfx_id	  bg_rp				= pfd.bind_group_gui_render_pass;
		const gfx_id	  bg_fullscreen		= pfd.bind_group_fullscreen;
		const gfx_id	  shader_fullscreen = _shaders.debug_controller_console_draw.get_hw();
		const uint16	  dc_count			= pfd.draw_call_count;

		render_pass_color_attachment* attachment_console_rt = alloc.allocate<render_pass_color_attachment>(1);
		attachment_console_rt->clear_color					= vector4(0.0f, 0.0f, 0.0f, 0.0f);
		attachment_console_rt->load_op						= load_op::clear;
		attachment_console_rt->store_op						= store_op::store;
		attachment_console_rt->texture						= rt_console;

		render_pass_color_attachment* attachment_fullscreen_rt = alloc.allocate<render_pass_color_attachment>(1);
		attachment_fullscreen_rt->clear_color				   = vector4(0.0f, 0.0f, 0.0f, 0.0f);
		attachment_fullscreen_rt->load_op					   = load_op::clear;
		attachment_fullscreen_rt->store_op					   = store_op::store;
		attachment_fullscreen_rt->texture					   = rt_fullscreen;

		gfx_backend* backend = gfx_backend::get();
		backend->cmd_bind_group(cmd_buffer, {.group = bg_rp});
		backend->cmd_set_viewport(cmd_buffer, {.width = static_cast<uint16>(rt_size.x), .height = static_cast<uint16>(rt_size.y)});

		backend->cmd_begin_render_pass(cmd_buffer, {.color_attachments = attachment_console_rt, .color_attachment_count = 1});
		backend->cmd_bind_vertex_buffers(cmd_buffer, {.buffer = gui_vertex, .vertex_size = sizeof(vekt::vertex)});
		backend->cmd_bind_index_buffers(cmd_buffer, {.buffer = gui_index, .bit_depth = 16});

		int32 last_pipeline = -1, last_bind_group = -1;

		for (uint16 i = 0; i < dc_count; i++)
		{
			gui_draw_call& dc = _gui_draw_calls[i];

			backend->cmd_set_scissors(cmd_buffer, {.x = dc.scissors.x, .y = dc.scissors.y, .width = dc.scissors.z, .height = dc.scissors.w});

			if (last_pipeline == -1 || last_pipeline != dc.shader)
			{
				backend->cmd_bind_pipeline(cmd_buffer, {.pipeline = dc.shader});
				last_pipeline = static_cast<int32>(dc.shader);
			}

			if (CHECK_BIT(dc.bind_group, 15) && (last_bind_group == -1 || last_bind_group != dc.bind_group))
			{
				const gfx_id bg = UNSET_BIT(dc.bind_group, 15);
				backend->cmd_bind_group(cmd_buffer, {.group = bg});
				last_bind_group = static_cast<int32>(dc.bind_group);
			}
			backend->cmd_draw_indexed_instanced(cmd_buffer, {.index_count_per_instance = dc.index_count, .instance_count = 1, .start_index_location = dc.start_idx, .base_vertex_location = dc.start_vtx, .start_instance_location = 0});
		}

		backend->cmd_end_render_pass(cmd_buffer, {});

		const barrier br_rt = {
			.resource	= rt_console,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::render_target,
			.to_state	= resource_state::ps_resource,
		};

		backend->cmd_barrier(cmd_buffer, {.barriers = &br_rt, .barrier_count = 1});

		backend->cmd_begin_render_pass(cmd_buffer, {.color_attachments = attachment_fullscreen_rt, .color_attachment_count = 1});
		backend->cmd_set_scissors(cmd_buffer, {.width = static_cast<uint16>(rt_size.x), .height = static_cast<uint16>(rt_size.y)});
		backend->cmd_set_viewport(cmd_buffer, {.width = static_cast<uint16>(rt_size.x), .height = static_cast<uint16>(rt_size.y)});

		// fullscreen draw
		backend->cmd_bind_group(cmd_buffer, {.group = bg_fullscreen});
		backend->cmd_bind_pipeline(cmd_buffer, {.pipeline = shader_fullscreen});
		backend->cmd_draw_instanced(cmd_buffer, {.vertex_count_per_instance = 6, .instance_count = 1, .start_vertex_location = 0, .start_instance_location = 0});

		backend->cmd_end_render_pass(cmd_buffer, {});

		const barrier br_rt_fs = {
			.resource	= rt_fullscreen,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::render_target,
			.to_state	= resource_state::ps_resource,
		};
		backend->cmd_barrier(cmd_buffer, {.barriers = &br_rt_fs, .barrier_count = 1});
	}

	void debug_controller::on_draw(const vekt::draw_buffer& buffer)
	{
		gfx_backend* backend = gfx_backend::get();

		const vekt::font*	  font			   = buffer.used_font;
		const vekt::atlas*	  atlas			   = font ? font->_atlas : nullptr;
		const vekt::font_type font_type		   = font ? font->type : vekt::font_type::normal;
		const vekt::vertex*	  buffer_vtx_start = buffer.vertex_start;
		const vekt::index*	  buffer_idx_start = buffer.index_start;
		const vector4		  clip			   = buffer.clip;
		const uint32		  buffer_idx_count = buffer.index_count;
		const uint32		  buffer_vtx_count = buffer.vertex_count;

		per_frame_data& pfd			= _pfd[_gfx_data.frame_index];
		const uint32	vtx_counter = pfd.counter_vtx;
		const uint32	idx_counter = pfd.counter_idx;
		const uint32	dc_count	= pfd.draw_call_count;
		pfd.draw_call_count++;
		pfd.counter_vtx += buffer_vtx_count;
		pfd.counter_idx += buffer_idx_count;
		pfd.buf_gui_vtx.buffer_data(sizeof(vekt::vertex) * static_cast<size_t>(vtx_counter), buffer_vtx_start, static_cast<size_t>(buffer_vtx_count) * sizeof(vekt::vertex));
		pfd.buf_gui_idx.buffer_data(sizeof(vekt::index) * static_cast<size_t>(idx_counter), buffer_idx_start, static_cast<size_t>(buffer_idx_count) * sizeof(vekt::index));
		SFG_ASSERT(pfd.draw_call_count < MAX_GUI_DRAW_CALLS);

		gui_draw_call& dc = _gui_draw_calls[dc_count];
		dc				  = {};
		dc.start_idx	  = idx_counter;
		dc.start_vtx	  = vtx_counter;
		dc.index_count	  = static_cast<uint32>(buffer_idx_count);

		if (clip.y < 0.0f)
		{
			dc.scissors.y = 0;
			dc.scissors.w = static_cast<uint16>(math::max(0.0f, clip.w + clip.y));
		}
		else
		{
			dc.scissors.y = static_cast<uint16>(clip.y);
			dc.scissors.w = static_cast<uint16>(clip.w);
		}

		if (clip.x < 0.0f)
		{
			dc.scissors.x = 0;
			dc.scissors.z = math::min((uint16)0, static_cast<uint16>(clip.x + clip.z));
		}
		else
		{
			dc.scissors.x = static_cast<uint16>(clip.x);
			dc.scissors.z = static_cast<uint16>(clip.z);
		}

		if (font)
		{
			dc.shader = font_type == vekt::font_type::sdf ? _shaders.gui_sdf.get_hw() : _shaders.gui_text.get_hw();
			auto it	  = vector_util::find_if(_gfx_data.atlases, [&](const atlas_ref& ref) -> bool { return ref.atlas == atlas; });
			SFG_ASSERT(it != _gfx_data.atlases.end());
			dc.bind_group = SET_BIT(it->bind_group, 15);
		}
		else
		{
			dc.shader = _shaders.gui_default.get_hw();
		}
	}

	void debug_controller::on_atlas_created(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		gfx_backend* backend = gfx_backend::get();
		_gfx_data.atlases.push_back({});

		atlas_ref& ref = _gfx_data.atlases.back();
		ref.atlas	   = atlas;
		ref.texture	   = backend->create_texture({
			   .texture_format = atlas->get_is_lcd() ? format::r8g8b8a8_srgb : format::r8_unorm,
			   .size		   = vector2ui16(static_cast<uint16>(atlas->get_width()), static_cast<uint16>(atlas->get_height())),
			   .flags		   = texture_flags::tf_is_2d | texture_flags::tf_sampled,
			   .views =
				   {
					   {},
				   },
			   .debug_name = "vekt_atlas",
		   });

		const uint32 txt_size	   = backend->get_texture_size(atlas->get_width(), atlas->get_height(), atlas->get_is_lcd() ? 3 : 1);
		const uint32 adjusted_size = backend->align_texture_size(txt_size);
		ref.intermediate_buffer	   = backend->create_resource({
			   .size	   = adjusted_size,
			   .flags	   = resource_flags::rf_cpu_visible,
			   .debug_name = "inter_buffer",
		   });

		ref.res_alive  = true;
		ref.bind_group = backend->create_empty_bind_group();
		backend->bind_group_add_pointer(ref.bind_group, rpi_table_material, 3, false);
		backend->bind_group_update_pointer(ref.bind_group, 0, {{.resource = ref.texture, .view = 0, .pointer_index = upi_material_texture0, .type = binding_type::texture_binding}});
	}

	void debug_controller::on_atlas_updated(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		gfx_backend* backend = gfx_backend::get();

		auto it = vector_util::find_if(_gfx_data.atlases, [atlas](const atlas_ref& ref) -> bool { return ref.atlas == atlas; });
		SFG_ASSERT(it != _gfx_data.atlases.end());
		atlas_ref& ref = *it;

		const unsigned char* data		  = atlas->get_data();
		const unsigned int	 size		  = atlas->get_data_size();
		const bool			 is_lcd		  = atlas->get_is_lcd();
		const uint32		 atlas_width  = atlas->get_width();
		const uint32		 atlas_height = atlas->get_height();
		const uint8			 bpp		  = atlas->get_is_lcd() ? 3 : 1;

		uint32 adjusted_size = 0;
		ref.buffer.pixels	 = reinterpret_cast<uint8*>(backend->adjust_buffer_pitch((void*)data, atlas_width, atlas_height, bpp, adjusted_size));
		ref.buffer.size		 = vector2ui16(static_cast<uint16>(atlas_width), static_cast<uint16>(atlas_height));
		ref.buffer.bpp		 = bpp;

		_gfx_data.texture_queue->add_request({
			.texture	  = ref.texture,
			.intermediate = ref.intermediate_buffer,
			.buffers	  = &ref.buffer,
			.buffer_count = 1,
		});
		ref.res_alive = true;

		const int32 index  = std::distance(it, _gfx_data.atlases.begin());
		uint8*		pixels = ref.buffer.pixels;
		_gfx_data.texture_queue->subscribe_flush_callback([index, this, pixels]() {
			PUSH_MEMORY_CATEGORY("Gfx");

			atlas_ref& ref = _gfx_data.atlases[index];
			ref.res_alive  = false;
			delete[] pixels;
			POP_MEMORY_CATEGORY();
		});
	}

	void debug_controller::on_atlas_destroyed(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		auto it = vector_util::find_if(_gfx_data.atlases, [atlas](const atlas_ref& ref) -> bool { return ref.atlas == atlas; });
		SFG_ASSERT(it != _gfx_data.atlases.end());
		atlas_ref&	 ref	 = *it;
		gfx_backend* backend = gfx_backend::get();

		backend->destroy_texture(ref.texture);
		backend->destroy_bind_group(ref.bind_group);

		backend->destroy_resource(ref.intermediate_buffer);

		if (ref.res_alive)
		{
			delete[] ref.buffer.pixels;
		}
		_gfx_data.atlases.erase(it);
	}

	void debug_controller::set_console_visible(bool visible)
	{
		_vekt_data.builder->widget_set_visible(_vekt_data.widget_console_bg, visible);
		_vekt_data.builder->widget_set_visible(_vekt_data.widget_input_field, visible);
		_vekt_data.builder->widget_set_visible(_vekt_data.widget_border, visible);
	}

	void debug_controller::console_logic()
	{
		_gfx_data.frame_counter++;
		if (_gfx_data.frame_counter % 120 == 0)
		{
			const vekt::text_props& fps_props	  = _vekt_data.builder->widget_get_text(_vekt_data.widget_fps);
			const vekt::text_props& update_props  = _vekt_data.builder->widget_get_text(_vekt_data.widget_main_thread);
			const vekt::text_props& render_props  = _vekt_data.builder->widget_get_text(_vekt_data.widget_render_thread);
			const vekt::text_props& present_props = _vekt_data.builder->widget_get_text(_vekt_data.widget_present_time);
			string_util::append_float(static_cast<float>(frame_info::get_fps()), (char*)fps_props.text + 5, 4, 1, true);
			string_util::append_float(static_cast<float>(frame_info::get_main_thread_time_milli()), (char*)update_props.text + 18, 7, 4, true);
			string_util::append_float(static_cast<float>(frame_info::get_render_thread_time_milli()), (char*)render_props.text + 20, 7, 4, true);
			string_util::append_float(static_cast<float>(frame_info::get_present_time_milli()), (char*)present_props.text + 14, 7, 4, true);

#ifdef ENABLE_MEMORY_TRACER
			memory_tracer& tracer = memory_tracer::get();
			LOCK_GUARD(tracer.get_category_mtx());

			const vekt::text_props& glob_mem_props = _vekt_data.builder->widget_get_text(_vekt_data.widget_global_mem);
			const vekt::text_props& gfx_mem_props  = _vekt_data.builder->widget_get_text(_vekt_data.widget_gfx_mem);

			for (const memory_category& cat : tracer.get_categories())
			{
				if (TO_SIDC(cat.name) == TO_SIDC("General"))
				{
					string_util::append_float(static_cast<float>(cat.total_size) / B_TO_MB, (char*)glob_mem_props.text + 18, 6, 4, true);
				}
				else if (TO_SIDC(cat.name) == TO_SIDC("Gfx"))
				{
					string_util::append_float(static_cast<float>(cat.total_size) / B_TO_MB, (char*)gfx_mem_props.text + 17, 6, 4, true);
				}
			}
#endif
		}

		if (_console_state == console_state::invisible)
			return;

		vekt::pos_props&  console_bg_pos_props	= _vekt_data.builder->widget_get_pos_props(_vekt_data.widget_console_bg);
		vekt::size_props& console_bg_size_props = _vekt_data.builder->widget_get_size_props(_vekt_data.widget_console_bg);
		const vector2&	  console_bg_size		= _vekt_data.builder->widget_get_size(_vekt_data.widget_console_bg);
		const vector2	  pos_text				= _vekt_data.builder->widget_get_pos(_vekt_data.widget_input_text);
		const vector2	  size_text				= _vekt_data.builder->widget_get_size(_vekt_data.widget_input_text);
		const vector2	  pos_field				= _vekt_data.builder->widget_get_pos(_vekt_data.widget_input_field);
		const float		  total_element_size	= _vekt_data.console_total_text_size_y;
		const float		  diff					= total_element_size - (console_bg_size.y - console_bg_size_props.child_margins.top - console_bg_size_props.child_margins.bottom);
		_input_field.scroll_amt					= math::clamp(_input_field.scroll_amt, (int16)0, static_cast<int16>(diff));
		console_bg_pos_props.scroll_offset		= -math::max(diff - _input_field.scroll_amt, 0.0f);

		vekt::widget_gfx gfx = {};

		const float size_per_char = _input_field.text_size == 0 ? 0 : (size_text.x / static_cast<float>(_input_field.text_size));

		const vector2					pos	  = vector2(pos_text.x + (size_per_char * static_cast<float>(_input_field.caret_pos)), pos_field.y + INPUT_FIELD_HEIGHT * 0.25f);
		const vekt::builder::rect_props props = {
			.gfx			 = gfx,
			.min			 = pos,
			.max			 = vector2(pos.x + INPUT_FIELD_HEIGHT * 0.25f, pos.y + INPUT_FIELD_HEIGHT * 0.5f),
			.use_hovered	 = false,
			.use_pressed	 = false,
			.color_start	 = COLOR_TEXT,
			.color_end		 = {},
			.color_direction = vekt::direction::horizontal,
			.widget_id		 = 0,
			.multi_color	 = false,
		};

		_vekt_data.builder->add_filled_rect(props);
	}

	void debug_controller::update_console_input_field()
	{
		_vekt_data.builder->widget_update_text(_vekt_data.widget_input_text);
		_input_field.text_size = strlen(_input_field.text);
		_input_field.caret_pos = math::min(_input_field.caret_pos, _input_field.text_size);
	}

	void debug_controller::on_log(log_level lvl, const char* msg)
	{
		add_console_text(msg, lvl);
	}

	void debug_controller::add_console_text(const char* text, log_level level)
	{
		if (level == log_level::trace)
			return;

		_input_field.scroll_amt = 0.0f;

		if (_vekt_data.console_texts.size() == MAX_CONSOLE_TEXT)
		{
			vekt::id		  t	 = _vekt_data.console_texts[0];
			vekt::text_props& tp = _vekt_data.builder->widget_get_text(t);
			_vekt_data.console_total_text_size_y -= _vekt_data.builder->widget_get_size_props(t).size.y + CONSOLE_SPACING;
			_text_allocator.deallocate((char*)tp.text);
			_vekt_data.builder->deallocate(t);
			_vekt_data.console_texts.erase(_vekt_data.console_texts.begin());
		}

		vekt::id w = _vekt_data.builder->allocate();
		_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f));

		vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
		gfx.flags			  = vekt::gfx_flags::gfx_is_text;
		gfx.color			  = COLOR_TEXT;

		switch (level)
		{
		case log_level::error:
			gfx.color = COLOR_TEXT_ERR;
			break;
		case log_level::warning:
			gfx.color = COLOR_TEXT_WARN;
			break;
		case log_level::progress:
			gfx.color = COLOR_TEXT_PROGRESS;
			break;
		default:
			gfx.color = COLOR_TEXT;
		}

		vekt::text_props& tp = _vekt_data.builder->widget_get_text(w);
		tp.text				 = _text_allocator.allocate(text);
		tp.font				 = _vekt_data.font_debug;
		_vekt_data.builder->widget_update_text(w);
		_vekt_data.builder->widget_add_child(_vekt_data.widget_console_bg, w);

		_vekt_data.console_texts.push_back(w);
		_vekt_data.console_total_text_size_y += _vekt_data.builder->widget_get_size_props(w).size.y + CONSOLE_SPACING;
	}

	void debug_controller::flush_key_events()
	{
		input_event ev = {};

		while (_input_events.try_dequeue(ev))
		{

			if (ev.wheel != 0)
			{
				_input_field.scroll_amt += ev.wheel * 50;
				continue;
			}

			const input_code button = static_cast<input_code>(ev.button);

			if (button == input_code::KeyAngleBracket)
			{
				if (_console_state == console_state::visible)
				{
					_console_state = console_state::invisible;
					set_console_visible(false);
					while (_input_events.pop())
						continue;
					return;
				}
				else if (_console_state == console_state::invisible)
				{
					_console_state = console_state::visible;
					set_console_visible(true);

					continue;
				}
			}

			_input_field.text_size = static_cast<int8>(strlen(_input_field.text));
			char* buffer		   = const_cast<char*>(_input_field.text);

			if (button == input_code::KeyBackspace)
			{
				if (_input_field.text_size != 0)
				{
					for (int i = _input_field.caret_pos; i < _input_field.text_size - 1; i++)
						buffer[i] = buffer[i + 1];

					buffer[_input_field.text_size - 1] = '\0';
					update_console_input_field();
				}

				continue;
			}

			if (button == input_code::KeyReturn)
			{
				if (_input_field.text_size > 0)
				{
					add_console_text(buffer, log_level::info);
					if (_input_field.history.size() >= MAX_HISTORY)
					{
						const char* history = _input_field.history[0];
						_text_allocator.deallocate((char*)history);
						_input_field.history.erase(_input_field.history.begin());
					}

					const char* history_element = _text_allocator.allocate(buffer);
					_input_field.history.push_back(history_element);
					_input_field.history_traversal = static_cast<int8>(_input_field.history.size());

					const size_t buffer_sz = strlen(buffer);
					const char*	 cmd	   = (const char*)SFG_MALLOC(buffer_sz);
					SFG_MEMCPY((char*)cmd, buffer, buffer_sz);
					_commands.emplace(cmd);

					buffer[0] = '\0';
					update_console_input_field();
				}

				continue;
			}

			if (button == input_code::KeyUp)
			{
				if (_input_field.history.empty())
					continue;

				_input_field.history_traversal = math::max(_input_field.history_traversal - 1, 0);

				const char* history = _input_field.history[_input_field.history_traversal];
				strcpy(buffer, history);
				update_console_input_field();
				_input_field.caret_pos = _input_field.text_size;

				continue;
				continue;
			}

			if (button == input_code::KeyDown)
			{
				if (_input_field.history.empty())
					continue;

				_input_field.history_traversal = math::min((int8)(_input_field.history_traversal + 1), static_cast<int8>(_input_field.history.size() - 1));

				const char* history = _input_field.history[_input_field.history_traversal];
				strcpy(buffer, history);
				update_console_input_field();
				_input_field.caret_pos = _input_field.text_size;

				continue;
			}

			if (button == input_code::KeyLeft)
			{
				_input_field.caret_pos = math::max(0, _input_field.caret_pos - 1);
				continue;
			}

			if (button == input_code::KeyRight)
			{
				_input_field.caret_pos = math::min(static_cast<int8>(_input_field.text_size), static_cast<int8>(_input_field.caret_pos + 1));
				continue;
			}

			if (_input_field.text_size >= MAX_INPUT_FIELD)
				continue;

			const char	 c	  = process::get_character_from_key(static_cast<uint32>(ev.button));
			const uint16 mask = process::get_character_mask_from_key(static_cast<uint32>(ev.button), c);

			if (!(mask & character_mask::printable))
				continue;

			for (int i = _input_field.text_size; i > _input_field.caret_pos; --i)
			{
				buffer[i] = buffer[i - 1];
			}

			buffer[_input_field.caret_pos]	   = c;
			buffer[_input_field.text_size + 1] = '\0';

			_input_field.caret_pos++;
			_input_field.text_size++;
			update_console_input_field();
		}
	}

	bool debug_controller::on_window_event(const window_event& ev)
	{
		if (ev.type == window_event_type::key && ev.sub_type != window_event_sub_type::release)
		{
			if (_console_state == console_state::invisible && static_cast<input_code>(ev.button) != input_code::KeyAngleBracket)
				return false;

			const input_event ke = {.button = static_cast<uint16>(ev.button)};
			_input_events.try_enqueue(ke);
			return true;
		}

		if (ev.type == window_event_type::wheel)
		{
			const input_event ke = {.wheel = ev.value.y};
			_input_events.try_enqueue(ke);
			return true;
		}

		return false;
	}

	void debug_controller::on_window_resize(const vector2ui16& size)
	{
		PUSH_MEMORY_CATEGORY("Gfx");
		gfx_backend* backend  = gfx_backend::get();
		_gfx_data.rt_size	  = vector2ui16(size.x, size.y / 2);
		_gfx_data.window_size = vector2ui16(size.x, size.y);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_texture(pfd.rt_console);
			backend->destroy_texture(pfd.rt_fullscreen);

			pfd.rt_console = backend->create_texture({
				.texture_format = RT_FORMAT,
				.size			= vector2ui16(size.x, size.y / 2),
				.flags			= texture_flags::tf_sampled | texture_flags::tf_is_2d | texture_flags::tf_render_target,
				.clear_values	= {0.0f, 0.0f, 0.0f, 0.0f},
				.debug_name		= "console_rt",
			});

			pfd.rt_fullscreen = backend->create_texture({
				.texture_format = RT_FORMAT,
				.size			= vector2ui16(size.x, size.y),
				.flags			= texture_flags::tf_sampled | texture_flags::tf_is_2d | texture_flags::tf_render_target,
				.clear_values	= {0.0f, 0.0f, 0.0f, 0.0f},
				.debug_name		= "debug_rt",
			});

			backend->bind_group_update_pointer(pfd.bind_group_fullscreen,
											   0,
											   {
												   {.resource = pfd.rt_console, .pointer_index = upi_material_texture0, .type = binding_type::texture_binding},
											   });
		}

		POP_MEMORY_CATEGORY();
	}
}
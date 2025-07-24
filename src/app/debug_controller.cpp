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
#include "gfx/gfx_util.hpp"
#include "gfx/texture_queue.hpp"
#include "common/system_info.hpp"
#include "platform/window_common.hpp"
#include "io/log.hpp"
#include "math/math.hpp"
#include "math/color.hpp"
#include "platform/process.hpp"
#include "input/input_mappings.hpp"

#define VEKT_STRING_CSTR
#define VEKT_VEC4 Game::vector4
#define VEKT_VEC2 Game::vector2
#define VEKT_IMPL
#include "gui/vekt.hpp"

namespace Game
{

#define MAX_CONSOLE_TEXT   128
#define MAX_INPUT_FIELD	   127
#define COLOR_TEXT		   color::srgb_to_linear(color(89.0f / 255.0f, 180.0f / 255.0f, 108.0f / 255.0f, 1.0f)).to_vector()
#define COLOR_CONSOLE_BG   color::srgb_to_linear(color(12.0f / 255.0f, 16.0f / 255.0f, 12.0f / 255.0f, 0.98f)).to_vector()
#define COLOR_BORDER	   color::srgb_to_linear(color(89.0f / 255.0f, 180.0f / 255.0f, 108.0f / 255.0f, 1.0f)).to_vector()
#define DEBUG_FONT_SIZE	   16
#define INPUT_FIELD_HEIGHT static_cast<float>(DEBUG_FONT_SIZE) * 1.5f
#define CONSOLE_SPACING	   static_cast<float>(DEBUG_FONT_SIZE) * 0.5f
#define MAX_HISTORY		   8

	constexpr float SDF_PARAMS[2] = {0.35f, 0.02f};

	void debug_controller::init(texture_queue* texture_queue)
	{
		_gfx_resources.texture_queue = texture_queue;

		gfx_backend* backend = gfx_backend::get();

		_gfx_resources.txt_dummy = backend->create_texture({
			.texture_format = format::r8_unorm,
			.size			= vector2ui16(1, 1),
			.flags			= texture_flags::tf_sampled | texture_flags::tf_is_2d,
		});

		_gfx_resources.bind_layout_gui = backend->create_empty_bind_layout();
		backend->bind_layout_add_descriptor(_gfx_resources.bind_layout_gui, binding_type::ubo, 0, 0, shader_stage::vertex);
		backend->bind_layout_add_constant(_gfx_resources.bind_layout_gui, 2, 0, 0, shader_stage::fragment);
		backend->bind_layout_add_pointer(_gfx_resources.bind_layout_gui,
										 {{
											 .type		  = binding_type::texture,
											 .set		  = 0,
											 .binding	  = 0,
											 .count		  = 1,
											 .is_volatile = false,
										 }},
										 shader_stage::fragment);
		backend->bind_layout_add_immutable_sampler(_gfx_resources.bind_layout_gui, 0, 0, gfx_util::get_sampler_desc_gui_text(), shader_stage::fragment);
		backend->finalize_bind_layout(_gfx_resources.bind_layout_gui, false);

		// gui default
		_shaders.gui_default.get_desc().attachments = {{.format = format::r8g8b8a8_srgb, .blend_attachment = gfx_util::get_blend_attachment_alpha_blending()}};
		_shaders.gui_default.get_desc().inputs		= gfx_util::get_input_layout(input_layout_type::gui_default);
		_shaders.gui_default.get_desc().cull		= cull_mode::back;
		_shaders.gui_default.get_desc().front		= front_face::cw;
		_shaders.gui_default.get_desc().layout		= _gfx_resources.bind_layout_gui;
		_shaders.gui_default.get_desc().set_name("gui_default");
		_shaders.gui_default.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_default.hlsl");

		// gui text
		_shaders.gui_text.get_desc().attachments = {{.format = format::r8g8b8a8_srgb, .blend_attachment = gfx_util::get_blend_attachment_alpha_blending()}};
		_shaders.gui_text.get_desc().inputs		 = gfx_util::get_input_layout(input_layout_type::gui_default);
		_shaders.gui_text.get_desc().cull		 = cull_mode::back;
		_shaders.gui_text.get_desc().front		 = front_face::cw;
		_shaders.gui_text.get_desc().layout		 = _gfx_resources.bind_layout_gui;
		_shaders.gui_text.get_desc().set_name("gui_text");
		_shaders.gui_text.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_text.hlsl");

		// gui sdf
		_shaders.gui_sdf.get_desc().attachments = {{.format = format::r8g8b8a8_srgb, .blend_attachment = gfx_util::get_blend_attachment_alpha_blending()}};
		_shaders.gui_sdf.get_desc().inputs		= gfx_util::get_input_layout(input_layout_type::gui_default);
		_shaders.gui_sdf.get_desc().cull		= cull_mode::back;
		_shaders.gui_sdf.get_desc().front		= front_face::cw;
		_shaders.gui_sdf.get_desc().layout		= _gfx_resources.bind_layout_gui;
		_shaders.gui_sdf.get_desc().set_name("gui_sdf");
		_shaders.gui_sdf.create_from_file_vertex_pixel("assets/engine/shaders/gui/gui_sdf.hlsl");

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			pfd.bind_group_gui_default = backend->create_empty_bind_group();
			backend->bind_group_add_descriptor(pfd.bind_group_gui_default, 0, binding_type::ubo);
			backend->bind_group_add_constant(pfd.bind_group_gui_default, 1, (uint8*)&SDF_PARAMS, 2);
			backend->bind_group_add_pointer(pfd.bind_group_gui_default, 2, 1, false);

			pfd.buf_gui_pass_view.create_hw({
				.size		= sizeof(gui_pass_view),
				.flags		= resource_flags::rf_cpu_visible,
				.debug_name = "cbv_gui_pass",
			});

			pfd.buf_gui_vtx.create_staging_hw(
				{
					.size		= sizeof(vekt::vertex) * 10000,
					.flags		= resource_flags::rf_vertex_buffer | resource_flags::rf_cpu_visible,
					.debug_name = "gui_vertex_stg",
				},
				{
					.size		= sizeof(vekt::vertex) * 10000,
					.flags		= resource_flags::rf_vertex_buffer | resource_flags::rf_gpu_only,
					.debug_name = "gui_vertex_gpu",
				});

			pfd.buf_gui_idx.create_staging_hw(
				{
					.size		= sizeof(vekt::index) * 12000,
					.flags		= resource_flags::rf_index_buffer | resource_flags::rf_cpu_visible,
					.debug_name = "gui_index_stg",
				},
				{
					.size		= sizeof(vekt::index) * 12000,
					.flags		= resource_flags::rf_index_buffer | resource_flags::rf_gpu_only,
					.debug_name = "gui_index_gpu",
				});

			backend->bind_group_update_descriptor(pfd.bind_group_gui_default, 0, pfd.buf_gui_pass_view.get_hw_gpu());
			backend->bind_group_update_pointer(pfd.bind_group_gui_default,
											   2,
											   {{
												   .resource = _gfx_resources.txt_dummy,
												   .view	 = 0,
												   .type	 = binding_type::texture,
											   }});
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
	}

	void debug_controller::build_console()
	{

		_input_field.text = _text_allocator.allocate(MAX_INPUT_FIELD);

		// vekt root
		{
			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(_vekt_data.builder->get_root());
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_column;
		}

		// Console parent
		{
			vekt::id w = _vekt_data.builder->allocate();
			_vekt_data.builder->widget_add_child(_vekt_data.builder->get_root(), w);
			_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f), vekt::helper_pos_type::relative, vekt::helper_pos_type::absolute);
			_vekt_data.builder->widget_set_size(w, vector2(1.0f, 0.5f));

			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_rect | vekt::gfx_flags::gfx_clip_children;
			gfx.color			  = COLOR_CONSOLE_BG;

			vekt::pos_props& pos_props = _vekt_data.builder->widget_get_pos_props(w);
			pos_props.flags			   = vekt::pos_flags::pf_child_pos_column;

			vekt::size_props& props	   = _vekt_data.builder->widget_get_size_props(w);
			props.child_margins.left   = CONSOLE_SPACING;
			props.child_margins.top	   = CONSOLE_SPACING * 2;
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
			vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
			gfx.flags			  = vekt::gfx_flags::gfx_is_rect;
			gfx.color			  = COLOR_BORDER;
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
	}

	void debug_controller::uninit()
	{
		vekt::font_manager::get().unload_font(_vekt_data.font_debug);
		vekt::font_manager::get().unload_font(_vekt_data.font_icon);
		vekt::font_manager::get().uninit();

		GAME_ASSERT(_gfx_resources.atlases.empty());

		_vekt_data.builder->uninit();
		delete _vekt_data.builder;

		gfx_backend* backend = gfx_backend::get();

		backend->destroy_texture(_gfx_resources.txt_dummy);

		_shaders.gui_default.destroy();
		_shaders.gui_text.destroy();
		_shaders.gui_sdf.destroy();
		backend->destroy_bind_layout(_gfx_resources.bind_layout_gui);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_bind_group(pfd.bind_group_gui_default);
			// backend->destroy_bind_group(pfd.bind_group_gui_sdf);
			backend->unmap_resource(pfd.buf_gui_pass_view.get_hw_gpu());
			backend->unmap_resource(pfd.buf_gui_vtx.get_hw_staging());
			backend->unmap_resource(pfd.buf_gui_idx.get_hw_staging());
			pfd.buf_gui_pass_view.destroy();
			pfd.buf_gui_vtx.destroy();
			pfd.buf_gui_idx.destroy();
		}
	}

	void debug_controller::upload(buffer_queue& q, uint8 frame_index, const vector2ui& size)
	{
		per_frame_data& pfd = _pfd[frame_index];
		pfd.reset();

		_gfx_resources.frame_index = frame_index;

		_vekt_data.builder->build_begin(vector2(size.x, size.y));
		console_logic();
		_vekt_data.builder->build_end();
		_vekt_data.builder->flush();

		q.add_request({.buffer = &pfd.buf_gui_idx});
		q.add_request({.buffer = &pfd.buf_gui_vtx});

		const gui_pass_view view = {
			.proj = matrix4x4::ortho(0, static_cast<float>(size.x), 0, static_cast<float>(size.y), 0.0f, 1.0f),
		};
		pfd.buf_gui_pass_view.buffer_data(0, (void*)&view, sizeof(gui_pass_view));
	}

	void debug_controller::render(resource_id cmd_buffer, resource_id render_target, uint8 frame_index, const vector2ui& size, bump_allocator& alloc)
	{
		gfx_backend*	backend = gfx_backend::get();
		per_frame_data& pfd		= _pfd[frame_index];

		// RENDER PASS
		{
			render_pass_color_attachment* attachment = alloc.allocate<render_pass_color_attachment>(1);
			attachment->clear_color					 = vector4(0.7f, 0.7f, 0.7f, 1.0f);
			attachment->load_op						 = load_op::clear;
			attachment->store_op					 = store_op::store;
			attachment->texture						 = render_target;

			backend->cmd_begin_render_pass_swapchain(cmd_buffer, {.color_attachments = attachment, .color_attachment_count = 1});

			backend->cmd_set_viewport(cmd_buffer, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_set_scissors(cmd_buffer, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_bind_vertex_buffers(cmd_buffer, {.buffer = pfd.buf_gui_vtx.get_hw_gpu(), .vertex_size = sizeof(vekt::vertex)});
			backend->cmd_bind_index_buffers(cmd_buffer, {.buffer = pfd.buf_gui_idx.get_hw_gpu(), .bit_depth = 16});

			int32 last_pipeline = -1, last_bind_group = -1;

			for (uint16 i = 0; i < pfd.draw_call_count; i++)
			{
				gui_draw_call& dc = pfd.draw_calls[i];

				backend->cmd_set_scissors(cmd_buffer, {.x = dc.scissors.x, .y = dc.scissors.y, .width = dc.scissors.z, .height = dc.scissors.w});

				if (last_pipeline == -1 || last_pipeline != dc.shader)
				{
					backend->cmd_bind_pipeline(cmd_buffer, {.pipeline = dc.shader});
					last_pipeline = static_cast<int32>(dc.shader);
					backend->cmd_bind_layout(cmd_buffer, {.layout = _gfx_resources.bind_layout_gui});
				}

				if (last_bind_group == -1 || last_bind_group != dc.bind_group)
				{
					backend->cmd_bind_group(cmd_buffer, {.group = dc.bind_group});
					last_bind_group = static_cast<int32>(dc.bind_group);
				}

				backend->cmd_draw_indexed_instanced(cmd_buffer, {.index_count_per_instance = dc.index_count, .instance_count = 1, .start_index_location = dc.start_idx, .base_vertex_location = dc.start_vtx, .start_instance_location = 0});
			}

			backend->cmd_end_render_pass(cmd_buffer, {});
		}
	}

	void debug_controller::console_logic()
	{
		vekt::pos_props& console_bg_pos_props = _vekt_data.builder->widget_get_pos_props(_vekt_data.widget_console_bg);
		const vector2&	 console_bg_size	  = _vekt_data.builder->widget_get_size(_vekt_data.widget_console_bg);
		const vector2	 pos_text			  = _vekt_data.builder->widget_get_pos(_vekt_data.widget_input_text);
		const vector2	 size_text			  = _vekt_data.builder->widget_get_size(_vekt_data.widget_input_text);
		const vector2	 pos_field			  = _vekt_data.builder->widget_get_pos(_vekt_data.widget_input_field);
		const float		 total_element_size	  = _vekt_data.console_total_text_size_y;
		console_bg_pos_props.scroll_offset	  = -(math::max(total_element_size - console_bg_size.y, 0.0f) + CONSOLE_SPACING);

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

	void debug_controller::on_draw(const vekt::draw_buffer& buffer)
	{
		gfx_backend*	backend = gfx_backend::get();
		per_frame_data& pfd		= _pfd[_gfx_resources.frame_index];

		pfd.buf_gui_vtx.buffer_data(sizeof(vekt::vertex) * static_cast<size_t>(pfd.counter_vtx), buffer.vertex_start, static_cast<size_t>(buffer.vertex_count) * sizeof(vekt::vertex));
		pfd.buf_gui_idx.buffer_data(sizeof(vekt::index) * static_cast<size_t>(pfd.counter_idx), buffer.index_start, static_cast<size_t>(buffer.index_count) * sizeof(vekt::index));

		gui_draw_call& dc = pfd.draw_calls[pfd.draw_call_count];
		dc.start_idx	  = static_cast<uint32>(pfd.counter_idx);
		dc.start_vtx	  = static_cast<uint32>(pfd.counter_vtx);
		dc.index_count	  = static_cast<uint32>(buffer.index_count);

		if (buffer.clip.y < 0.0f)
		{
			dc.scissors.y = 0;
			dc.scissors.w = static_cast<uint16>(math::max(0.0f, buffer.clip.w + buffer.clip.y));
		}
		else
		{
			dc.scissors.y = static_cast<uint16>(buffer.clip.y);
			dc.scissors.w = static_cast<uint16>(buffer.clip.w);
		}

		if (buffer.clip.x < 0.0f)
		{
			dc.scissors.x = 0;
			dc.scissors.z = math::min((uint16)0, static_cast<uint16>(buffer.clip.x + buffer.clip.z));
		}
		else
		{
			dc.scissors.x = static_cast<uint16>(buffer.clip.x);
			dc.scissors.z = static_cast<uint16>(buffer.clip.z);
		}

		if (buffer.used_font)
		{
			dc.shader = buffer.used_font->type == vekt::font_type::sdf ? _shaders.gui_sdf.get_hw() : _shaders.gui_text.get_hw();
			auto it	  = vector_util::find_if(_gfx_resources.atlases, [&](const atlas_ref& ref) -> bool { return ref.atlas == buffer.used_font->_atlas; });
			GAME_ASSERT(it != _gfx_resources.atlases.end());
			dc.bind_group = it->bind_group[_gfx_resources.frame_index];
		}
		else
		{
			dc.shader	  = _shaders.gui_default.get_hw();
			dc.bind_group = pfd.bind_group_gui_default;
		}

		pfd.draw_call_count++;
		pfd.counter_vtx += buffer.vertex_count;
		pfd.counter_idx += buffer.index_count;
	}

	void debug_controller::on_atlas_created(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		gfx_backend* backend = gfx_backend::get();
		_gfx_resources.atlases.push_back({});

		atlas_ref& ref = _gfx_resources.atlases.back();
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

		const uint32 adjusted_size = backend->get_aligned_texture_size(atlas->get_width(), atlas->get_height(), atlas->get_is_lcd() ? 3 : 1);
		ref.intermediate_buffer	   = backend->create_resource({
			   .size	   = adjusted_size,
			   .flags	   = resource_flags::rf_cpu_visible,
			   .debug_name = "inter_buffer",
		   });

		ref.res_alive = true;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			resource_id&	bg	= ref.bind_group[i];

			bg = backend->create_empty_bind_group();

			backend->bind_group_add_descriptor(bg, 0, binding_type::ubo);
			backend->bind_group_add_constant(bg, 1, (uint8*)&SDF_PARAMS, 2);
			backend->bind_group_add_pointer(bg, 2, 1, false);

			backend->bind_group_update_descriptor(bg, 0, pfd.buf_gui_pass_view.get_hw_gpu());
			backend->bind_group_update_pointer(bg, 2, {{.resource = ref.texture, .view = 0, .type = binding_type::texture}});
		}
	}

	void debug_controller::on_atlas_updated(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		gfx_backend* backend = gfx_backend::get();

		auto it = vector_util::find_if(_gfx_resources.atlases, [atlas](const atlas_ref& ref) -> bool { return ref.atlas == atlas; });
		GAME_ASSERT(it != _gfx_resources.atlases.end());
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

		_gfx_resources.texture_queue->add_request({
			.texture	  = ref.texture,
			.intermediate = ref.intermediate_buffer,
			.buffers	  = &ref.buffer,
			.buffer_count = 1,
		});
		ref.res_alive = true;

		const int32 index  = std::distance(it, _gfx_resources.atlases.begin());
		uint8*		pixels = ref.buffer.pixels;
		_gfx_resources.texture_queue->subscribe_flush_callback([index, this, pixels]() {
			atlas_ref& ref = _gfx_resources.atlases[index];
			ref.res_alive  = false;
			delete[] pixels;
		});
	}

	void debug_controller::on_atlas_destroyed(vekt::atlas* atlas)
	{
		VERIFY_THREAD_MAIN();

		auto it = vector_util::find_if(_gfx_resources.atlases, [atlas](const atlas_ref& ref) -> bool { return ref.atlas == atlas; });
		GAME_ASSERT(it != _gfx_resources.atlases.end());
		atlas_ref&	 ref	 = *it;
		gfx_backend* backend = gfx_backend::get();

		backend->destroy_texture(ref.texture);
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			backend->destroy_bind_group(ref.bind_group[i]);

		backend->destroy_resource(ref.intermediate_buffer);

		if (ref.res_alive)
		{
			delete[] ref.buffer.pixels;
		}
		_gfx_resources.atlases.erase(it);
	}

	void debug_controller::add_console_text(const string& text)
	{
		if (_vekt_data.console_texts.size() == MAX_CONSOLE_TEXT)
		{
			vekt::id		  t	 = _vekt_data.console_texts[0];
			vekt::text_props& tp = _vekt_data.builder->widget_get_text(t);
			_text_allocator.deallocate((char*)tp.text);
			_vekt_data.builder->deallocate(t);
			_vekt_data.console_texts.erase(_vekt_data.console_texts.begin());
		}

		vekt::id w = _vekt_data.builder->allocate();
		_vekt_data.builder->widget_set_pos(w, vector2(0.0f, 0.0f));

		vekt::widget_gfx& gfx = _vekt_data.builder->widget_get_gfx(w);
		gfx.flags			  = vekt::gfx_flags::gfx_is_text;
		const color col		  = color::srgb_to_linear(color(89.0f / 255.0f, 180.0f / 255.0f, 108.0f / 255.0f, 1.0f));
		gfx.color			  = COLOR_TEXT;
		vekt::text_props& tp  = _vekt_data.builder->widget_get_text(w);
		tp.text				  = _text_allocator.allocate(text.c_str());
		tp.font				  = _vekt_data.font_debug;
		_vekt_data.builder->widget_update_text(w);
		_vekt_data.builder->widget_add_child(_vekt_data.widget_console_bg, w);

		_vekt_data.console_texts.push_back(w);
		_vekt_data.console_total_text_size_y += _vekt_data.builder->widget_get_size_props(w).size.y + CONSOLE_SPACING;
	}

	bool debug_controller::on_window_event(const window_event& ev)
	{
		if (ev.type == window_event_type::key && ev.sub_type != window_event_sub_type::release)
		{
			_input_field.text_size = static_cast<int8>(strlen(_input_field.text));
			char* buffer		   = const_cast<char*>(_input_field.text);

			if (ev.button == input_code::KeyBackspace)
			{
				if (_input_field.text_size != 0)
				{
					buffer[_input_field.text_size - 1] = '\0';
					update_console_input_field();
				}

				_input_field.caret_pos = math::max(0, _input_field.caret_pos - 1);
				return true;
			}

			if (ev.button == input_code::KeyReturn)
			{
				add_console_text(buffer);
				if (_input_field.history.size() >= MAX_HISTORY)
				{
					const char* history = _input_field.history[0];
					_text_allocator.deallocate((char*)history);
					_input_field.history.erase(_input_field.history.begin());
				}

				const char* history_element = _text_allocator.allocate(buffer);
				_input_field.history.push_back(history_element);
				_input_field.history_traversal = static_cast<int8>(_input_field.history.size());

				buffer[0] = '\0';
				update_console_input_field();

				return true;
			}

			if (ev.button == input_code::KeyUp)
			{
				if (_input_field.history.empty())
					return true;

				_input_field.history_traversal = math::max(_input_field.history_traversal - 1, 0);

				const char* history = _input_field.history[_input_field.history_traversal];
				strcpy(buffer, history);
				update_console_input_field();
				_input_field.caret_pos = _input_field.text_size;

				return true;
			}

			if (ev.button == input_code::KeyDown)
			{
				if (_input_field.history.empty())
					return true;

				_input_field.history_traversal = math::min((int8)(_input_field.history_traversal + 1), static_cast<int8>(_input_field.history.size() - 1));

				const char* history = _input_field.history[_input_field.history_traversal];
				strcpy(buffer, history);
				update_console_input_field();
				_input_field.caret_pos = _input_field.text_size;

				return true;
			}

			if (ev.button == input_code::KeyLeft)
			{
				_input_field.caret_pos = math::max(0, _input_field.caret_pos - 1);
				return true;
			}

			if (ev.button == input_code::KeyRight)
			{
				_input_field.caret_pos = math::min(static_cast<int8>(_input_field.text_size), static_cast<int8>(_input_field.caret_pos + 1));
				return true;
			}

			if (_input_field.text_size >= MAX_INPUT_FIELD)
				return true;

			const char c = process::get_character_from_key(static_cast<uint32>(ev.button));
			for (int i = _input_field.text_size; i > _input_field.caret_pos; --i)
			{
				buffer[i] = buffer[i - 1];
			}

			buffer[_input_field.caret_pos]	   = c;
			buffer[_input_field.text_size + 1] = '\0';

			_input_field.caret_pos++;
			_input_field.text_size++;
			update_console_input_field();
			return true;
		}

		return false;
	}
}
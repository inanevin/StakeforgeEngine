// Copyright (c) 2025 Inan Evin

#include "render_pass_lighting_forward.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/util/gfx_util.hpp"

namespace SFG
{
	void render_pass_lighting_forward::init(const init_data& data)
	{
		_alloc.init(data.alloc, data.alloc_size);
		gfx_backend* backend = gfx_backend::get();

		create_textures(data.size);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			pfd.cmd_buffer			= backend->create_command_buffer({.type = command_type::graphics, .debug_name = "opaque_cmd"});
			pfd.semaphore.semaphore = backend->create_semaphore();

			pfd.ubo_lighting.create_hw({.size = sizeof(ubo_lighting), .flags = resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible, .debug_name = "lighting_ubo"});
			pfd.ubo_forward.create_hw({.size = sizeof(ubo_forward), .flags = resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible, .debug_name = "forward_ubo"});

			const uint32 base		= i == 0 ? 0 : 4;
			pfd.bind_group_lighting = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group_lighting, rpi_table_render_pass, 10, false);
			backend->bind_group_update_pointer(pfd.bind_group_lighting,
											   0,
											   {
												   {.resource = pfd.ubo_lighting.get_hw_gpu(), .view = 0, .pointer_index = upi_render_pass_ubo0, .type = binding_type::ubo},
												   {.resource = data.light_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo0, .type = binding_type::ssbo},
												   {.resource = data.opaque_textures[base], .view = 0, .pointer_index = upi_render_pass_texture0, .type = binding_type::texture_binding},
												   {.resource = data.opaque_textures[base + 1], .view = 0, .pointer_index = upi_render_pass_texture1, .type = binding_type::texture_binding},
												   {.resource = data.opaque_textures[base + 2], .view = 0, .pointer_index = upi_render_pass_texture2, .type = binding_type::texture_binding},
												   {.resource = data.opaque_textures[base + 3], .view = 0, .pointer_index = upi_render_pass_texture3, .type = binding_type::texture_binding},
											   });

			pfd.bind_group_forward = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group_forward, rpi_table_render_pass, 6, false);
			backend->bind_group_update_pointer(pfd.bind_group_forward,
											   0,
											   {
												   {.resource = pfd.ubo_forward.get_hw_gpu(), .view = 0, .pointer_index = upi_render_pass_ubo0, .type = binding_type::ubo},
												   {.resource = data.entity_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo0, .type = binding_type::ssbo},
												   {.resource = data.bone_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo1, .type = binding_type::ssbo},
												   {.resource = data.light_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo2, .type = binding_type::ssbo},
											   });

			pfd.depth_texture = data.depth_textures[i];
		}
	}

	void render_pass_lighting_forward::uninit()
	{
		_alloc.uninit();

		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			backend->destroy_command_buffer(pfd.cmd_buffer);
			backend->destroy_bind_group(pfd.bind_group_lighting);
			backend->destroy_bind_group(pfd.bind_group_forward);
			backend->destroy_semaphore(pfd.semaphore.semaphore);
			pfd.ubo_lighting.destroy();
			pfd.ubo_forward.destroy();
		}

		destroy_textures();
	}

	void render_pass_lighting_forward::reset_target_textures(gfx_id* opaque_textures, gfx_id* depth_textures)
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			pfd.depth_texture	= depth_textures[i];
			const uint32 base	= i == 0 ? 0 : 4;

			backend->bind_group_update_pointer(pfd.bind_group_lighting,
											   0,
											   {
												   {.resource = opaque_textures[base], .view = 0, .pointer_index = upi_render_pass_texture0, .type = binding_type::texture_binding},
												   {.resource = opaque_textures[base + 1], .view = 0, .pointer_index = upi_render_pass_texture1, .type = binding_type::texture_binding},
												   {.resource = opaque_textures[base + 2], .view = 0, .pointer_index = upi_render_pass_texture2, .type = binding_type::texture_binding},
												   {.resource = opaque_textures[base + 3], .view = 0, .pointer_index = upi_render_pass_texture3, .type = binding_type::texture_binding},
											   });
		}
	}

	void render_pass_lighting_forward::populate_render_data(world* w, uint8 data_index)
	{
	}

	void render_pass_lighting_forward::render(uint8 data_index, uint8 frame_index, const vector2ui16& size, gfx_id global_layout, gfx_id global_group)
	{
		gfx_backend*	backend				   = gfx_backend::get();
		per_frame_data& pfd					   = _pfd[frame_index];
		render_data&	rd					   = _render_data[data_index];
		const gfx_id*	textures			   = pfd.color_textures.data();
		const gfx_id	cmd_buffer			   = pfd.cmd_buffer;
		const gfx_id	depth_texture		   = pfd.depth_texture;
		const gfx_id	rp_bind_group_lighting = pfd.bind_group_lighting;
		const gfx_id	rp_bind_group_forward  = pfd.bind_group_forward;

		render_pass_color_attachment* attachments = _alloc.allocate<render_pass_color_attachment>(COLOR_TEXTURES);

		for (uint8 i = 0; i < COLOR_TEXTURES; i++)
		{
			render_pass_color_attachment& att = attachments[i];
			att.clear_color					  = vector4(1, 0, 0, 1.0f);
			att.load_op						  = load_op::clear;
			att.store_op					  = store_op::store;
			att.texture						  = textures[i];
		}

		backend->cmd_begin_render_pass_depth(cmd_buffer,
											 {
												 .color_attachments = attachments,
												 .depth_stencil_attachment =
													 {
														 .texture		 = depth_texture,
														 .clear_stencil	 = 0,
														 .clear_depth	 = 1.0f,
														 .depth_load_op	 = load_op::clear,
														 .depth_store_op = store_op::store,
														 .view_index	 = 0,
													 },
												 .color_attachment_count = COLOR_TEXTURES,
											 });

		backend->cmd_bind_layout(cmd_buffer, {.layout = global_layout});
		backend->cmd_bind_group(cmd_buffer, {.group = global_group});
		backend->cmd_bind_group(cmd_buffer, {.group = rp_bind_group_lighting});
		backend->cmd_set_scissors(cmd_buffer, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
		backend->cmd_set_viewport(cmd_buffer, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});

		gfx_id last_bound_group	   = std::numeric_limits<gfx_id>::max();
		gfx_id last_bound_pipeline = std::numeric_limits<gfx_id>::max();

		auto bind = [&](gfx_id group, gfx_id pipeline) {
			if (pipeline != last_bound_pipeline)
			{
				last_bound_pipeline = pipeline;
				backend->cmd_bind_pipeline(cmd_buffer, {.pipeline = pipeline});
			}

			if (group != last_bound_group)
			{
				last_bound_group = group;
				backend->cmd_bind_group(cmd_buffer, {.group = group});
			}
		};

		for (const indexed_draw& draw : rd.draws)
		{
			bind(draw.bind_group, draw.pipeline);

			backend->cmd_bind_constants(cmd_buffer, {.data = (void*)&draw.constants, .offset = 0, .count = 4});
			backend->cmd_draw_indexed_instanced(cmd_buffer,
												{
													.index_count_per_instance = draw.index_count,
													.instance_count			  = draw.instance_count,
													.start_index_location	  = draw.start_index,
													.base_vertex_location	  = draw.base_vertex,
													.start_instance_location  = draw.start_instance,
												});
		}

		backend->cmd_end_render_pass(cmd_buffer, {});
	}

	void render_pass_lighting_forward::resize(const vector2ui16& size)
	{
		destroy_textures();
		create_textures(size);
	}

	void render_pass_lighting_forward::destroy_textures()
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			for (uint32 i = 0; i < COLOR_TEXTURES; i++)
				backend->destroy_texture(pfd.color_textures[i]);
			pfd.color_textures.clear();
		}
	}

	void render_pass_lighting_forward::create_textures(const vector2ui16& sz)
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			pfd.color_textures.clear();

			for (uint32 j = 0; j < COLOR_TEXTURES; j++)
			{
				pfd.color_textures.push_back(backend->create_texture({
					.texture_format = format::r16g16b16a16_sfloat,
					.size			= sz,
					.flags			= texture_flags::tf_render_target | texture_flags::tf_is_2d,
				}));
			}
		}
	}
}

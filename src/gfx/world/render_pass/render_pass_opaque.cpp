// Copyright (c) 2025 Inan Evin

#include "render_pass_opaque.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/util/gfx_util.hpp"
#include "gfx/buffer_queue.hpp"
#include "gfx/world/world_render_data.hpp"
#include "world/world.hpp"

namespace SFG
{
	void render_pass_opaque::init(const init_params& params)
	{
		_alloc.init(params.alloc, params.alloc_size);

		gfx_backend* backend = gfx_backend::get();

		create_textures(params.size);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			pfd.cmd_buffer			= backend->create_command_buffer({.type = command_type::graphics, .debug_name = "opaque_cmd"});
			pfd.semaphore.semaphore = backend->create_semaphore();

			pfd.ubo.create_hw({.size = sizeof(ubo), .flags = resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible, .debug_name = "opaque_ubo"});

			pfd.bind_group = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group, rpi_table_render_pass, 5, false);
			backend->bind_group_update_pointer(pfd.bind_group,
											   0,
											   {
												   {.resource = pfd.ubo.get_hw_gpu(), .view = 0, .pointer_index = upi_render_pass_ubo0, .type = binding_type::ubo},
												   {.resource = params.entity_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo0, .type = binding_type::ssbo},
												   {.resource = params.bone_buffers[i], .view = 0, .pointer_index = upi_render_pass_ssbo1, .type = binding_type::ssbo},
											   });
		}
	}

	void render_pass_opaque::uninit()
	{
		_alloc.uninit();

		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			backend->destroy_command_buffer(pfd.cmd_buffer);
			backend->destroy_bind_group(pfd.bind_group);
			backend->destroy_semaphore(pfd.semaphore.semaphore);
			pfd.ubo.destroy();
		}

		destroy_textures();
	}

	void render_pass_opaque::populate_render_data(world* w, const view& v, const world_render_data& wd, uint8 data_index)
	{
		render_data& rd = _render_data[data_index];
		rd.proj			= v.proj_matrix;
		rd.view			= v.view_matrix;
		rd.view_proj	= v.view_proj_matrix;
		rd.draws.clear();

		world_resources& resources = w->get_resources();

		for (const renderable_object& obj : wd.renderables)
		{
			const material&		  mat	= resources.get_material(obj.material);
			const bitmask<uint8>& flags = mat.get_flags();

			if (!flags.is_set(material::flags::is_opaque))
				continue;

			rd.draws.push_back({
				.constants =
					{
						.constant0 = obj.gpu_entity,
					},
				.base_vertex	= obj.vertex_start,
				.index_count	= obj.index_count,
				.instance_count = 1,
				.start_index	= obj.index_start,
				.start_instance = 0,
				.pipeline		= mat.get_shader(resources, obj.is_skinned ? shader::flags::is_skinned : 0),
			});
		}
	}

	void render_pass_opaque::upload(world* w, buffer_queue* queue, uint8 data_index, uint8 frame_index)
	{
		per_frame_data& pfd = _pfd[frame_index];
		render_data&	rd	= _render_data[data_index];

		ubo ubo_data = {
			.view	   = rd.view,
			.proj	   = rd.proj,
			.view_proj = rd.view_proj,
		};

		pfd.ubo.buffer_data(0, &ubo_data, sizeof(ubo));
		queue->add_request({.buffer = &pfd.ubo});
	}

	void render_pass_opaque::render(uint8 data_index, uint8 frame_index, const vector2ui16& size, gfx_id global_layout, gfx_id global_group)
	{
		gfx_backend*	backend		  = gfx_backend::get();
		per_frame_data& pfd			  = _pfd[frame_index];
		render_data&	rd			  = _render_data[data_index];
		const gfx_id*	textures	  = pfd.color_textures.data();
		const gfx_id	cmd_buffer	  = pfd.cmd_buffer;
		const gfx_id	depth_texture = pfd.depth_texture;
		const gfx_id	rp_bind_group = pfd.bind_group;
		_alloc.reset();

		render_pass_color_attachment* attachments = _alloc.allocate<render_pass_color_attachment>(COLOR_TEXTURES);

		static_vector<barrier, COLOR_TEXTURES + 1> barriers;
		static_vector<barrier, COLOR_TEXTURES + 1> barriers_after;

		for (uint8 i = 0; i < COLOR_TEXTURES; i++)
		{
			const gfx_id txt = textures[i];

			render_pass_color_attachment& att = attachments[i];
			att.clear_color					  = vector4(0, 0, 0, 1.0f);
			att.load_op						  = load_op::clear;
			att.store_op					  = store_op::store;
			att.texture						  = txt;

			barriers.push_back({
				.resource	= txt,
				.flags		= barrier_flags::baf_is_texture,
				.from_state = resource_state::ps_resource,
				.to_state	= resource_state::render_target,
			});

			barriers_after.push_back({
				.resource	= txt,
				.flags		= barrier_flags::baf_is_texture,
				.from_state = resource_state::render_target,
				.to_state	= resource_state::ps_resource,
			});
		}

		barriers.push_back({
			.resource	= depth_texture,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::common,
			.to_state	= resource_state::depth_write,
		});

		barriers_after.push_back({
			.resource	= depth_texture,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::depth_write,
			.to_state	= resource_state::common,
		});

		backend->reset_command_buffer(cmd_buffer);

		backend->cmd_barrier(cmd_buffer,
							 {
								 .barriers		= barriers.data(),
								 .barrier_count = static_cast<uint16>(barriers.size()),
							 });

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
		backend->cmd_bind_group(cmd_buffer, {.group = rp_bind_group});
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

		backend->cmd_barrier(cmd_buffer,
							 {
								 .barriers		= barriers_after.data(),
								 .barrier_count = static_cast<uint16>(barriers_after.size()),
							 });

		backend->close_command_buffer(cmd_buffer);
	}

	void render_pass_opaque::resize(const vector2ui16& size)
	{
		destroy_textures();
		create_textures(size);
	}

	void render_pass_opaque::destroy_textures()
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			for (uint32 i = 0; i < COLOR_TEXTURES; i++)
				backend->destroy_texture(pfd.color_textures[i]);
			backend->destroy_texture(pfd.depth_texture);
			pfd.color_textures.clear();
		}
	}

	void render_pass_opaque::create_textures(const vector2ui16& sz)
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			pfd.color_textures.clear();

			for (uint32 j = 0; j < COLOR_TEXTURES; j++)
			{
				pfd.color_textures.push_back(backend->create_texture({
					.texture_format = format::r8g8b8a8_srgb,
					.size			= sz,
					.flags			= texture_flags::tf_render_target | texture_flags::tf_is_2d | texture_flags::tf_sampled,
					.debug_name		= "opaque_color",
				}));
			}

			pfd.depth_texture = backend->create_texture({
				.texture_format		  = format::d16_unorm,
				.depth_stencil_format = format::d16_unorm,
				.size				  = sz,
				.flags				  = texture_flags::tf_depth_texture | texture_flags::tf_is_2d,
				.debug_name			  = "opaque_depth",
			});
		}
	}
}

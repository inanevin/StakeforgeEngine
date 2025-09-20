// Copyright (c) 2025 Inan Evin
#include "render_pass.hpp"
#include "math/vector2ui.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/common/format.hpp"
#include "memory/bump_allocator.hpp"

namespace SFG
{
	void render_pass::create_color_targets(uint8 target_count, uint8 fmt, const vector2ui16& size)
	{
		SFG_ASSERT(target_count < MAX_RP_TEXTURES);

		gfx_backend* backend = gfx_backend::get();
		_texture_format		 = fmt;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			for (uint8 i = 0; i < target_count; i++)
			{
				const gfx_id txt = backend->create_texture({
					.texture_format = static_cast<format>(fmt),
					.size			= size,
					.flags			= texture_flags::tf_render_target | texture_flags::tf_is_2d,
				});

				pfd.textures.push_back(txt);
			}
		}
		_flags.set(render_pass_flags::owns_color);
	}

	void render_pass::create_depth_target(uint8 fmt, const vector2ui16& size)
	{
		gfx_backend* backend = gfx_backend::get();

		_depth_format		   = fmt;
		const format depth_fmt = static_cast<format>(fmt);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			pfd.depth_texture = backend->create_texture({
				.texture_format		  = depth_fmt,
				.depth_stencil_format = depth_fmt,
				.size				  = size,
				.flags				  = texture_flags::tf_depth_texture | texture_flags::tf_is_2d,
			});
		}

		_flags.set(render_pass_flags::owns_depth);
	}

	void render_pass::set_color_targets(uint8 target_count, gfx_id* targets)
	{
		SFG_ASSERT(!_flags.is_set(render_pass_flags::owns_color));

		uint32 texture_index = 0;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			for (uint8 i = 0; i < target_count; i++)
			{
				pfd.textures.push_back(targets[texture_index]);
				texture_index++;
			}
		}
	}

	void render_pass::set_depth_target(gfx_id* targets)
	{
		SFG_ASSERT(!_flags.is_set(render_pass_flags::owns_depth));

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			pfd.depth_texture	= targets[i];
		}
	}

	void render_pass::set_bind_group(uint8 frame_index, gfx_id group)
	{
		per_frame_data& pfd = _pfd[frame_index];
		pfd.bind_group		= group;
	}

	void render_pass::uninit()
	{
		gfx_backend* backend   = gfx_backend::get();
		const format depth_fmt = static_cast<format>(_depth_format);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];

			if (_flags.is_set(render_pass_flags::owns_color))
			{
				for (gfx_id id : pfd.textures)
					backend->destroy_texture(id);

				pfd.textures.clear();
			}

			if (_flags.is_set(render_pass_flags::owns_depth) && depth_fmt != format::undefined)
				backend->destroy_texture(pfd.depth_texture);
		}
	}

	void render_pass::render(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size)
	{
		gfx_backend*	backend	 = gfx_backend::get();
		per_frame_data& pfd		 = _pfd[frame_index];
		const gfx_id*	textures = pfd.textures.data();

		const uint8 texture_count = static_cast<uint8>(pfd.textures.size());

		render_pass_color_attachment* attachments = alloc.allocate<render_pass_color_attachment>(texture_count);

		for (uint8 i = 0; i < texture_count; i++)
		{
			render_pass_color_attachment& att = attachments[i];
			att.clear_color					  = vector4(1, 0, 0, 1.0f);
			att.load_op						  = load_op::clear;
			att.store_op					  = store_op::store;
			att.texture						  = textures[i];
		}

		backend->cmd_begin_render_pass(cmd_buffer,
									   {
										   .color_attachments	   = attachments,
										   .color_attachment_count = texture_count,
									   });
		draw(draws, draws_count, alloc, cmd_buffer, frame_index, size);

		backend->cmd_end_render_pass(cmd_buffer, {});
	}

	void render_pass::render_w_depth(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size)
	{
		gfx_backend*	backend		  = gfx_backend::get();
		per_frame_data& pfd			  = _pfd[frame_index];
		const gfx_id*	textures	  = pfd.textures.data();
		const gfx_id	depth_texture = pfd.depth_texture;
		const uint8		texture_count = static_cast<uint8>(pfd.textures.size());

		render_pass_color_attachment* attachments = alloc.allocate<render_pass_color_attachment>(texture_count);

		for (uint8 i = 0; i < texture_count; i++)
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
												 .color_attachment_count = texture_count,
											 });

		draw(draws, draws_count, alloc, cmd_buffer, frame_index, size);

		backend->cmd_end_render_pass(cmd_buffer, {});
	}

	void render_pass::draw(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size)
	{
		gfx_backend*	backend = gfx_backend::get();
		per_frame_data& pfd		= _pfd[frame_index];

		backend->cmd_bind_group(cmd_buffer, {.group = pfd.bind_group});
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

		for (uint32 i = 0; i < draws_count; i++)
		{
			indexed_draw& draw = draws[i];
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
	}

	void render_pass::resize(const vector2ui16& size)
	{
		gfx_backend* backend = gfx_backend::get();

		const format txt_fmt   = static_cast<format>(_texture_format);
		const format depth_fmt = static_cast<format>(_depth_format);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd			  = _pfd[i];
			const uint8		texture_count = static_cast<uint8>(pfd.textures.size());

			if (_flags.is_set(render_pass_flags::owns_color))
			{
				for (gfx_id id : pfd.textures)
					backend->destroy_texture(id);

				pfd.textures.clear();

				for (uint8 i = 0; i < texture_count; i++)
				{
					const gfx_id txt = backend->create_texture({
						.texture_format = static_cast<format>(txt_fmt),
						.size			= size,
						.flags			= texture_flags::tf_render_target | texture_flags::tf_is_2d,
					});

					pfd.textures.push_back(txt);
				}
			}

			if (_flags.is_set(render_pass_flags::owns_depth) && depth_fmt != format::undefined)
			{
				backend->destroy_texture(pfd.depth_texture);

				pfd.depth_texture = backend->create_texture({
					.texture_format		  = depth_fmt,
					.depth_stencil_format = depth_fmt,
					.size				  = size,
					.flags				  = texture_flags::tf_depth_texture | texture_flags::tf_is_2d,
				});
			}
		}
	}

}
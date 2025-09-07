// Copyright (c) 2025 Inan Evin

#include "world_renderer.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/util/gfx_util.hpp"
#include "memory/memory_tracer.hpp"
#include "game/world.hpp"
#include "gfx/texture_queue.hpp"
#include "common/system_info.hpp"

namespace SFG
{
#define RT_FORMAT	 format::r8g8b8a8_srgb
#define DEPTH_FORMAT format::d16_unorm

	void world_renderer::init(const vector2ui16& size, texture_queue* tq, buffer_queue* bq)
	{
		PUSH_MEMORY_CATEGORY("Gfx");
		_texture_queue = tq;
		_buffer_queue  = bq;

		gfx_backend* backend = gfx_backend::get();

		const bitmask16 color_flags = texture_flags::tf_is_2d | texture_flags::tf_render_target | texture_flags::tf_sampled;

		_base_size = size;
		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd	  = _pfd[i];
			pfd.sem_gfx.semaphore = backend->create_semaphore();
			pfd.render_target	  = backend->create_texture({
					.texture_format = RT_FORMAT,
					.size			= _base_size,
					.flags			= color_flags,
					.debug_name		= "wr_color",
			});

			pfd.depth_target = backend->create_texture({
				.texture_format		  = DEPTH_FORMAT,
				.depth_stencil_format = DEPTH_FORMAT,
				.size				  = _base_size,
				.flags				  = texture_flags::tf_is_2d | texture_flags::tf_depth_texture,
				.debug_name			  = "wr_depth",
			});

			pfd.gfx_buffer = backend->create_command_buffer({.type = command_type::graphics, .debug_name = "wr_cmd_list"});
			pfd.allocator.init(1024 * 1024, 8);
		}

		const uint32 vertex_buffer_size = 36000;
		const uint32 index_buffer_size	= 36000;
		_render_data.big_vertex_buffer.create_staging_hw(
			{
				.size		= vertex_buffer_size,
				.flags		= resource_flags::rf_cpu_visible,
				.debug_name = "big_vertex_staging",
			},
			{
				.size		= vertex_buffer_size,
				.flags		= resource_flags::rf_vertex_buffer | resource_flags::rf_gpu_only,
				.debug_name = "big_vertex_gpu",
			});

		_render_data.big_index_buffer.create_staging_hw(
			{
				.size		= index_buffer_size,
				.flags		= resource_flags::rf_cpu_visible,
				.debug_name = "big_index_staging",
			},
			{
				.size		= index_buffer_size,
				.flags		= resource_flags::rf_index_buffer | resource_flags::rf_gpu_only,
				.debug_name = "big_index_gpu",
			});

		_reuse_uploaded_textures.reserve(MAX_WORLD_TEXTURES);
		_reuse_pending_textures.reserve(MAX_WORLD_TEXTURES);
		_reuse_pending_models.reserve(MAX_WORLD_MODELS);

		POP_MEMORY_CATEGORY();
	}

	void world_renderer::uninit()
	{
		PUSH_MEMORY_CATEGORY("Gfx");

		gfx_backend* backend = gfx_backend::get();

		_render_data.big_vertex_buffer.destroy();
		_render_data.big_index_buffer.destroy();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_semaphore(pfd.sem_gfx.semaphore);
			backend->destroy_texture(pfd.render_target);
			backend->destroy_texture(pfd.depth_target);
			backend->destroy_command_buffer(pfd.gfx_buffer);
			pfd.allocator.uninit();
		}
		POP_MEMORY_CATEGORY();
	}

	void world_renderer::populate_render_data(uint8 index, double interpolation)
	{
		check_uploads();
	}

	void world_renderer::on_render_joined()
	{
		check_uploads(true);
	}

	void world_renderer::upload(uint8 data_index, uint8 frame_index)
	{
		for (texture* t : _reuse_pending_textures)
		{
			_texture_queue->add_request({
				.texture	  = t->get_hw(),
				.intermediate = t->get_intermediate(),
				.buffers	  = t->get_cpu(),
				.buffer_count = t->get_cpu_count(),
			});
			_reuse_uploaded_textures.push_back(t);
			_last_upload_frame = frame_info::get_render_frame();
		}

		for (model* mdl : _reuse_pending_models)
		{
			const vector<mesh>& meshes = mdl->get_meshes();

			for (mesh m : meshes)
			{
				for (primitive_static& p : m.primitives_static)
				{
					const size_t sz = sizeof(primitive_index) * p.indices.size();
					_render_data.big_index_buffer.buffer_data(_render_data.current_index_size, p.indices.data(), sz);
					_render_data.current_index_size += sz;

					const size_t vsz = sizeof(vertex_static) * p.vertices.size();
					_render_data.big_vertex_buffer.buffer_data(_render_data.current_vertex_size, p.vertices.data(), vsz);
					_render_data.current_vertex_size += sz;
				}

				for (primitive_skinned& p : m.primitives_skinned)
				{
					const size_t sz = sizeof(primitive_index) * p.indices.size();
					_render_data.big_index_buffer.buffer_data(_render_data.current_index_size, p.indices.data(), sz);
					_render_data.current_index_size += sz;

					const size_t vsz = sizeof(vertex_skinned) * p.vertices.size();
					_render_data.big_vertex_buffer.buffer_data(_render_data.current_vertex_size, p.vertices.data(), vsz);
					_render_data.current_vertex_size += sz;
				}
			}
		}

		_reuse_pending_textures.resize(0);
		_reuse_pending_models.resize(0);
	}

	void world_renderer::render(uint8 data_index, uint8 frame_index, gfx_id layout_global, gfx_id bind_group_global)
	{
		gfx_backend* backend   = gfx_backend::get();
		const gfx_id queue_gfx = backend->get_queue_gfx();

		per_frame_data& pfd				  = _pfd[frame_index];
		const gfx_id	cmd_buf_gfx		  = pfd.gfx_buffer;
		const gfx_id	base_color_target = pfd.render_target;
		const gfx_id	base_depth_target = pfd.depth_target;
		bump_allocator& alloc			  = pfd.allocator;
		alloc.reset();

		backend->reset_command_buffer(cmd_buf_gfx);
		backend->cmd_bind_layout(cmd_buf_gfx, {.layout = layout_global});
		backend->cmd_bind_group(cmd_buf_gfx, {.group = bind_group_global});

		_reuse_barriers.push_back({
			.resource	= base_color_target,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::ps_resource,
			.to_state	= resource_state::render_target,
		});

		// _reuse_barriers.push_back({
		// 	.resource	= base_depth_target,
		// 	.flags		= barrier_flags::baf_is_texture,
		// 	.from_state = resource_state::common,
		// 	.to_state	= resource_state::depth_write,
		// });

		send_barriers(cmd_buf_gfx);

		{
			render_pass_color_attachment* attachment = alloc.allocate<render_pass_color_attachment>(1);
			attachment->clear_color					 = vector4(1, 0, 0, 1.0f);
			attachment->load_op						 = load_op::clear;
			attachment->store_op					 = store_op::store;
			attachment->texture						 = base_color_target;

			backend->cmd_begin_render_pass(cmd_buf_gfx, {.color_attachments = attachment, .color_attachment_count = 1});
			backend->cmd_set_scissors(cmd_buf_gfx, {.width = static_cast<uint16>(_base_size.x), .height = static_cast<uint16>(_base_size.y)});
			backend->cmd_set_viewport(cmd_buf_gfx, {.width = static_cast<uint16>(_base_size.x), .height = static_cast<uint16>(_base_size.y)});
			backend->cmd_end_render_pass(cmd_buf_gfx, {});
		}

		_reuse_barriers.push_back({
			.resource	= base_color_target,
			.flags		= barrier_flags::baf_is_texture,
			.from_state = resource_state::render_target,
			.to_state	= resource_state::ps_resource,
		});

		//_reuse_barriers.push_back({
		//	.resource	= base_depth_target,
		//	.flags		= barrier_flags::baf_is_texture,
		//	.from_state = resource_state::depth_write,
		//	.to_state	= resource_state::depth_read,
		//});

		send_barriers(cmd_buf_gfx);

		backend->close_command_buffer(cmd_buf_gfx);
	}

	void world_renderer::submit(uint8 frame_index, uint64 prev_copy_value, const semaphore_data& copy_semaphore)
	{
		gfx_backend*	backend		  = gfx_backend::get();
		const gfx_id	queue_gfx	  = backend->get_queue_gfx();
		per_frame_data& pfd			  = _pfd[frame_index];
		const gfx_id	cmd_buf_gfx	  = pfd.gfx_buffer;
		const gfx_id	sem_gfx		  = pfd.sem_gfx.semaphore;
		const uint64	sem_gfx_value = ++pfd.sem_gfx.value;

		if (prev_copy_value != copy_semaphore.value)
			backend->queue_wait(queue_gfx, &copy_semaphore.semaphore, &copy_semaphore.value, 1);

		backend->submit_commands(queue_gfx, &cmd_buf_gfx, 1);
		backend->queue_signal(queue_gfx, &sem_gfx, &sem_gfx_value, 1);
	}

	void world_renderer::resize(const vector2ui16& size)
	{
		_base_size			 = size;
		gfx_backend* backend = gfx_backend::get();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_texture(pfd.render_target);
			backend->destroy_texture(pfd.depth_target);

			pfd.render_target = backend->create_texture({
				.texture_format = RT_FORMAT,
				.size			= _base_size,
				.flags			= texture_flags::tf_is_2d | texture_flags::tf_render_target | texture_flags::tf_sampled,
				.debug_name		= "wr_color",
			});

			pfd.depth_target = backend->create_texture({
				.texture_format		  = DEPTH_FORMAT,
				.depth_stencil_format = DEPTH_FORMAT,
				.size				  = _base_size,
				.flags				  = texture_flags::tf_is_2d | texture_flags::tf_depth_texture,
				.debug_name			  = "wr_depth",
			});
		}
	}

	void world_renderer::add_pending_texture(texture* txt)
	{
		_reuse_pending_textures.push_back(txt);
	}

	void world_renderer::add_pending_model(model* mdl)
	{
		_reuse_pending_models.push_back(mdl);
	}

	void world_renderer::check_uploads(bool force)
	{
		if (_reuse_uploaded_textures.empty())
			return;

		const uint64 current_frame = frame_info::get_render_frame();
		if (force || current_frame > _last_upload_frame.load() + FRAMES_IN_FLIGHT + 2)
		{
			for (texture* txt : _reuse_uploaded_textures)
			{
				txt->destroy_cpu();
				txt->destroy_intermediate();
			}
			_reuse_uploaded_textures.resize(0);
		}
	}

	void world_renderer::send_barriers(gfx_id cmd_list)
	{
		gfx_backend* backend = gfx_backend::get();
		backend->cmd_barrier(cmd_list,
							 {
								 .barriers		= _reuse_barriers.data(),
								 .barrier_count = static_cast<uint16>(_reuse_barriers.size()),
							 });

		_reuse_barriers.resize(0);
	}
}

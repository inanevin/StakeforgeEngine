// Copyright (c) 2025 Inan Evin

#include "world_resource_uploads.hpp"
#include "common/system_info.hpp"
#include "gfx/texture_queue.hpp"
#include "gfx/buffer_queue.hpp"
#include "resources/texture.hpp"
#include "resources/model.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "world/common_world.hpp"

namespace SFG
{
	void world_resource_uploads::init(texture_queue* tq, buffer_queue* bq)
	{
		gfx_backend* backend			= gfx_backend::get();
		const uint32 vertex_buffer_size = 36000;
		const uint32 index_buffer_size	= 36000;
		_mesh_data.big_vertex_buffer.create_staging_hw(
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

		_mesh_data.big_index_buffer.create_staging_hw(
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
	}
	void world_resource_uploads::uninit()
	{
		gfx_backend* backend = gfx_backend::get();

		_mesh_data.big_vertex_buffer.destroy();
		_mesh_data.big_index_buffer.destroy();
	}

	void world_resource_uploads::upload(uint8 data_index, uint8 frame_index)
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
			vector<mesh>& meshes = mdl->get_meshes();

			for (mesh& m : meshes)
			{
				for (primitive_static& p : m.primitives_static)
				{
					p.runtime.vertex_start = _mesh_data.current_vertex_size;
					p.runtime.index_start  = _mesh_data.current_index_size;

					const size_t sz = sizeof(primitive_index) * p.indices.size();
					_mesh_data.big_index_buffer.buffer_data(_mesh_data.current_index_size, p.indices.data(), sz);
					_mesh_data.current_index_size += sz;

					const size_t vsz = sizeof(vertex_static) * p.vertices.size();
					_mesh_data.big_vertex_buffer.buffer_data(_mesh_data.current_vertex_size, p.vertices.data(), vsz);
					_mesh_data.current_vertex_size += sz;
				}

				for (primitive_skinned& p : m.primitives_skinned)
				{
					p.runtime.vertex_start = _mesh_data.current_vertex_size;
					p.runtime.index_start  = _mesh_data.current_index_size;

					const size_t sz = sizeof(primitive_index) * p.indices.size();
					_mesh_data.big_index_buffer.buffer_data(_mesh_data.current_index_size, p.indices.data(), sz);
					_mesh_data.current_index_size += sz;

					const size_t vsz = sizeof(vertex_skinned) * p.vertices.size();
					_mesh_data.big_vertex_buffer.buffer_data(_mesh_data.current_vertex_size, p.vertices.data(), vsz);
					_mesh_data.current_vertex_size += sz;
				}
			}
		}

		if (!_reuse_pending_models.empty())
		{
			_buffer_queue->add_request({.buffer = &_mesh_data.big_vertex_buffer});
			_buffer_queue->add_request({.buffer = &_mesh_data.big_index_buffer});
		}

		_reuse_pending_textures.clear();
		_reuse_pending_models.clear();
	}

	void world_resource_uploads::add_pending_texture(texture* txt)
	{
		_reuse_pending_textures.push_back(txt);
	}

	void world_resource_uploads::add_pending_model(model* mdl)
	{
		_reuse_pending_models.push_back(mdl);
	}

	void world_resource_uploads::check_uploads(bool force)
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
			_reuse_uploaded_textures.clear();
		}
	}

}

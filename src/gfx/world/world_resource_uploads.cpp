// Copyright (c) 2025 Inan Evin

#include "world_resource_uploads.hpp"
#include "common/system_info.hpp"
#include "gfx/texture_queue.hpp"
#include "gfx/buffer_queue.hpp"
#include "resources/texture.hpp"
#include "resources/mesh.hpp"
#include "resources/material.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "world/common_world.hpp"
#include "memory/chunk_allocator.hpp"
#include "common/system_info.hpp"

namespace SFG
{
	void world_resource_uploads::init()
	{
		gfx_backend* backend			= gfx_backend::get();
		const uint32 vertex_buffer_size = 1024 * 1024 * 2;
		const uint32 index_buffer_size	= 1024 * 1024 * 2;
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

	void world_resource_uploads::upload(chunk_allocator32& resources_aux, texture_queue* tq, buffer_queue* bq, uint8 data_index, uint8 frame_index)
	{
		for (texture* t : _pending_textures)
		{
			tq->add_request({
				.texture	  = t->get_hw(),
				.intermediate = t->get_intermediate(),
				.buffers	  = t->get_cpu(),
				.buffer_count = t->get_cpu_count(),
			});
			_uploaded_textures.push_back(t);
			_last_upload_frame = frame_info::get_render_frame();
		}

		for (mesh* m : _pending_meshes)
		{
			const chunk_handle32 prims_static		= m->get_primitives_static();
			const uint16		 prims_static_count = m->get_primitives_static_count();
			primitive*			 ptr_static			= prims_static_count == 0 ? nullptr : resources_aux.get<primitive>(prims_static);

			for (uint16 j = 0; j < prims_static_count; j++)
			{
				primitive& p		   = ptr_static[j];
				p.runtime.vertex_start = _mesh_data.current_vertex_size;
				p.runtime.index_start  = _mesh_data.current_index_size;

				_mesh_data.big_index_buffer.buffer_data(_mesh_data.current_index_size, resources_aux.get(p.indices.head), p.indices.size);
				_mesh_data.current_index_size += p.indices.size;

				_mesh_data.big_vertex_buffer.buffer_data(_mesh_data.current_vertex_size, resources_aux.get(p.vertices.head), p.vertices.size);
				_mesh_data.current_vertex_size += p.vertices.size;
			}

			const chunk_handle32 prims_skinned		 = m->get_primitives_skinned();
			const uint16		 prims_skinned_count = m->get_primitives_skinned_count();
			primitive*			 ptr_skinned		 = prims_skinned_count == 0 ? nullptr : resources_aux.get<primitive>(prims_skinned);

			for (uint16 j = 0; j < prims_skinned_count; j++)
			{
				primitive& p = ptr_skinned[j];

				p.runtime.vertex_start = _mesh_data.current_vertex_size;
				p.runtime.index_start  = _mesh_data.current_index_size;

				_mesh_data.big_index_buffer.buffer_data(_mesh_data.current_index_size, resources_aux.get(p.indices.head), p.indices.size);
				_mesh_data.current_index_size += p.indices.size;

				_mesh_data.big_vertex_buffer.buffer_data(_mesh_data.current_vertex_size, resources_aux.get(p.vertices.head), p.vertices.size);
				_mesh_data.current_vertex_size += p.vertices.size;
			}
		}

		if (!_pending_meshes.empty())
		{
			bq->add_request({.buffer = &_mesh_data.big_vertex_buffer});
			bq->add_request({.buffer = &_mesh_data.big_index_buffer});
		}

		per_frame_data& pfd = _pfd[frame_index];

		for (material* mat : pfd.pending_materials)
		{
			buffer&		   buf	= mat->get_buffer(frame_index);
			const ostream& data = mat->get_data();
			buf.buffer_data(0, data.get_raw(), data.get_size());
		}

		pfd.pending_materials.clear();
		_pending_textures.clear();
		_pending_meshes.clear();
	}

	void world_resource_uploads::add_pending_texture(texture* txt)
	{
		VERIFY_RENDER_NOT_RUNNING_OR_RENDER_THREAD();
		_pending_textures.push_back(txt);
	}

	void world_resource_uploads::add_pending_material(material* mat)
	{
		VERIFY_RENDER_NOT_RUNNING_OR_RENDER_THREAD();

		for (uint8 i = 0; i < FRAMES_IN_FLIGHT; i++)
			_pfd[i].pending_materials.push_back(mat);
	}

	void world_resource_uploads::add_pending_mesh(mesh* m)
	{
		VERIFY_RENDER_NOT_RUNNING_OR_RENDER_THREAD();
		_pending_meshes.push_back(m);
	}

	void world_resource_uploads::check_uploads(bool force)
	{
		if (_uploaded_textures.empty())
			return;

		const uint64 current_frame = frame_info::get_render_frame();
		if (force || current_frame > _last_upload_frame.load() + FRAMES_IN_FLIGHT + 2)
		{
			for (texture* txt : _uploaded_textures)
			{
				txt->destroy_cpu();
				txt->destroy_intermediate();
			}
			_uploaded_textures.clear();
		}
	}

}

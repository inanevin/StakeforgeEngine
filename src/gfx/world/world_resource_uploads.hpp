// Copyright (c) 2025 Inan Evin

#pragma once

#include "gfx/buffer.hpp"
#include "data/static_vector.hpp"
#include "data/atomic.hpp"
#include "world/common_world.hpp"

namespace SFG
{

	class texture_queue;
	class buffer_queue;
	class world;
	class texture;
	class model;
	class material;
	class chunk_allocator32;

	class world_resource_uploads
	{
	private:
		struct mesh_data
		{
			buffer big_vertex_buffer   = {};
			buffer big_index_buffer	   = {};
			size_t current_vertex_size = 0;
			size_t current_index_size  = 0;
		};

		struct per_frame_data
		{
			static_vector<material*, MAX_WORLD_MATERIALS> pending_materials;
		};

	public:
		void init();
		void uninit();

		void add_pending_texture(texture* txt);
		void add_pending_material(material* matk);
		void add_pending_model(model* mdl);
		void upload(chunk_allocator32& resources_aux, texture_queue* tq, buffer_queue* bq, uint8 data_index, uint8 frame_index);
		void check_uploads(bool force = false);

		inline buffer& get_big_vertex_buffer()
		{
			return _mesh_data.big_vertex_buffer;
		}

		inline buffer& get_big_index_buffer()
		{
			return _mesh_data.big_index_buffer;
		}

	private:
	private:
		mesh_data									_mesh_data = {};
		per_frame_data								_pfd[FRAMES_IN_FLIGHT];
		static_vector<texture*, MAX_WORLD_TEXTURES> _pending_textures;
		static_vector<texture*, MAX_WORLD_TEXTURES> _uploaded_textures;
		static_vector<model*, MAX_WORLD_MODELS>		_pending_models;
		atomic<uint64>								_last_upload_frame = 0;
	};
}

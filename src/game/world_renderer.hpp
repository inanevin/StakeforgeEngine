// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/atomic.hpp"
#include "math/vector2ui16.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "gfx/common/barrier_description.hpp"
#include "memory/bump_allocator.hpp"
#include "gfx/buffer.hpp"

namespace SFG
{

	class texture_queue;
	class buffer_queue;
	class world;
	class texture;
	class model;

	class world_renderer
	{
	private:
		struct per_frame_data
		{
			semaphore_data sem_gfx		 = {};
			gfx_id		   render_target = 0;
			gfx_id		   depth_target	 = 0;
			gfx_id		   gfx_buffer	 = 0;
			bump_allocator allocator;
		};

		struct render_data
		{
			buffer big_vertex_buffer   = {};
			buffer big_index_buffer	   = {};
			size_t current_vertex_size = 0;
			size_t current_index_size  = 0;
		};

	public:
		void init(const vector2ui16& size, texture_queue* tq, buffer_queue* bq);
		void uninit();

		void on_render_joined();
		void populate_render_data(uint8 index, double interpolation);
		void upload(uint8 data_index, uint8 frame_index);
		void render(uint8 data_index, uint8 frame_index, gfx_id layout_global, gfx_id bind_group_global);
		void submit(uint8 frame_index, uint64 prev_copy_value, const semaphore_data& copy_semaphore);
		void resize(const vector2ui16& size);

		void add_pending_texture(texture* txt);
		void add_pending_model(model* mdl);

		inline void set_id(uint8 id)
		{
			_id = id;
		}

		inline gfx_id get_cmd_buffer(uint8 index)
		{
			return _pfd[index].gfx_buffer;
		}

		inline gfx_id get_output(uint8 frame_index)
		{
			return _pfd[frame_index].render_target;
		}

		inline const semaphore_data& get_gfx_semaphore(uint8 frame_index)
		{
			return _pfd[frame_index].sem_gfx;
		}

	private:
		void check_uploads(bool force = false);
		void send_barriers(gfx_id cmd_list);

	private:
		texture_queue*	 _texture_queue = nullptr;
		buffer_queue*	 _buffer_queue	= nullptr;
		world*			 _world			= nullptr;
		per_frame_data	 _pfd[FRAMES_IN_FLIGHT];
		render_data		 _render_data = {};
		vector<barrier>	 _reuse_barriers;
		vector<texture*> _reuse_uploaded_textures;
		vector<texture*> _reuse_pending_textures;
		vector<model*>	 _reuse_pending_models;
		vector2ui16		 _base_size			= vector2ui16::zero;
		atomic<uint64>	 _last_upload_frame = 0;
		uint8			 _id				= 0;
	};
}

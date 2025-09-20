// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/static_vector.hpp"
#include "data/atomic.hpp"
#include "math/vector2ui16.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/common/gfx_common.hpp"
#include "gfx/common/barrier_description.hpp"
#include "gfx/render_pass.hpp"
#include "gfx/buffer.hpp"
#include "memory/bump_allocator.hpp"
#include "world_resource_uploads.hpp"
#include "world_render_data.hpp"

#include "render_pass/render_pass_opaque.hpp"
#include "render_pass/render_pass_lighting_forward.hpp"
#include "render_pass/render_pass_post_combiner.hpp"

namespace SFG
{

	class texture_queue;
	class buffer_queue;
	class world;
	class texture;
	class model;

#define MAX_BARRIERS 20

	class world_renderer
	{
	private:
		struct per_frame_data
		{
			buffer		   bones	= {};
			buffer		   entities = {};
			buffer		   lights	= {};
			semaphore_data sem_gfx	= {};
		};

	public:
		void init(const vector2ui16& size, texture_queue* tq, buffer_queue* bq, world* w);
		void uninit();

		void on_render_joined();
		void populate_render_data(uint8 index, double interpolation);
		void upload(uint8 data_index, uint8 frame_index);
		void render(uint8 data_index, uint8 frame_index, gfx_id layout_global, gfx_id bind_group_global);
		void resize(const vector2ui16& size);

		uint32 create_gpu_entity(uint8 data_index, const gpu_entity& e);

		inline gfx_id get_output(uint8 frame_index)
		{
			// return _pass_post_combiner.get_color_texture(frame_index);
			return _pass_opaque.get_color_texture(frame_index, 0);
		}

		inline const semaphore_data& get_final_semaphore(uint8 frame_index)
		{
			// return _pass_post_combiner.get_semaphore(frame_index);
			return _pass_opaque.get_semaphore(frame_index);
		}

		inline world_resource_uploads& get_resource_uploads()
		{
			return _resource_uploads;
		}

	private:
		void push_barrier_ps(gfx_id id, static_vector<barrier, MAX_BARRIERS>& barriers);
		void push_barrier_rt(gfx_id id, static_vector<barrier, MAX_BARRIERS>& barriers);
		void send_barriers(gfx_id cmd_list, static_vector<barrier, MAX_BARRIERS>& barriers);

	private:
		texture_queue*	  _texture_queue					= nullptr;
		buffer_queue*	  _buffer_queue						= nullptr;
		world*			  _world							= nullptr;
		world_render_data _render_data[THREAD_BUFFER_COUNT] = {};

		render_pass_opaque			 _pass_opaque		 = {};
		render_pass_lighting_forward _pass_lighting_fw	 = {};
		render_pass_post_combiner	 _pass_post_combiner = {};

		per_frame_data		   _pfd[FRAMES_IN_FLIGHT];
		world_resource_uploads _resource_uploads;
		vector2ui16			   _base_size			 = vector2ui16::zero;
		uint8*				   _shared_command_alloc = nullptr;
	};
}

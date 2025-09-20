// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/static_vector.hpp"
#include "gfx/render_pass.hpp"
#include "gfx/common/barrier_description.hpp"
#include "gfx/buffer.hpp"
#include "gfx/world/draws.hpp"
#include "memory/bump_allocator.hpp"

namespace SFG
{
	class world;

	class render_pass_lighting_forward
	{
	private:
		static constexpr uint32 MAX_DRAWS	   = 512;
		static constexpr uint32 MAX_BARRIERS   = 16;
		static constexpr uint32 COLOR_TEXTURES = 1;

		struct ubo_lighting
		{
		};

		struct ubo_forward
		{
		};

		struct per_frame_data
		{
			gfx_id								  cmd_buffer;
			gfx_id								  bind_group_lighting;
			gfx_id								  bind_group_forward;
			buffer								  ubo_lighting;
			buffer								  ubo_forward;
			semaphore_data						  semaphore;
			static_vector<gfx_id, COLOR_TEXTURES> color_textures;
			gfx_id								  depth_texture = 0;
			static_vector<barrier, MAX_BARRIERS>  barriers		= {};
		};

		struct render_data
		{
			static_vector<indexed_draw, MAX_DRAWS> draws;
		};

	public:
		struct init_data
		{
			const vector2ui16& size;
			uint8*			   alloc;
			size_t			   alloc_size;
			gfx_id*			   entity_buffers;
			gfx_id*			   bone_buffers;
			gfx_id*			   light_buffers;
			gfx_id*			   opaque_textures;
			gfx_id*			   depth_textures;
		};

		void init(const init_data& data);
		void uninit();
		void reset_target_textures(gfx_id* opaque_textures, gfx_id* depth_textures);
		void populate_render_data(world* w, uint8 data_index);
		void render(uint8 data_index, uint8 frame_index, const vector2ui16& size, gfx_id global_layout, gfx_id global_group);
		void resize(const vector2ui16& size);

		inline semaphore_data& get_semaphore(uint8 frame_index)
		{
			return _pfd[frame_index].semaphore;
		}

		inline gfx_id get_cmd_buffer(uint8 frame_index) const
		{
			return _pfd[frame_index].cmd_buffer;
		}

	private:
		void destroy_textures();
		void create_textures(const vector2ui16& sz);

	private:
		render_pass	   _pass = {};
		per_frame_data _pfd[FRAMES_IN_FLIGHT];
		render_data	   _render_data[THREAD_BUFFER_COUNT];
		bump_allocator _alloc = {};
	};
}

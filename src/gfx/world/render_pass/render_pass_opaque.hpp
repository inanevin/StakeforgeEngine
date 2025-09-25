// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/static_vector.hpp"
#include "gfx/render_pass.hpp"
#include "gfx/common/barrier_description.hpp"
#include "gfx/buffer.hpp"
#include "gfx/world/draws.hpp"
#include "memory/bump_allocator.hpp"
#include "math/matrix4x4.hpp"

namespace SFG
{
	class world;
	struct world_render_data;
	class buffer_queue;
	class world;
	struct vector2ui16;
	struct view;

	class render_pass_opaque
	{
	private:
		static constexpr uint32 MAX_DRAWS	   = 512;
		static constexpr uint32 MAX_BARRIERS   = 16;
		static constexpr uint32 COLOR_TEXTURES = 4;

		struct ubo
		{
			matrix4x4 view		= matrix4x4::identity;
			matrix4x4 proj		= matrix4x4::identity;
			matrix4x4 view_proj = matrix4x4::identity;
		};

		struct per_frame_data
		{
			gfx_id								  cmd_buffer;
			gfx_id								  bind_group;
			buffer								  ubo;
			semaphore_data						  semaphore;
			static_vector<gfx_id, COLOR_TEXTURES> color_textures;
			gfx_id								  depth_texture = 0;
			static_vector<barrier, MAX_BARRIERS>  barriers		= {};
		};

		struct render_data
		{
			static_vector<indexed_draw, MAX_DRAWS> draws;
			matrix4x4							   view		 = matrix4x4::identity;
			matrix4x4							   proj		 = matrix4x4::identity;
			matrix4x4							   view_proj = matrix4x4::identity;
		};

	public:
		struct init_params
		{
			const vector2ui16& size;
			uint8*			   alloc;
			size_t			   alloc_size;
			gfx_id*			   entity_buffers;
			gfx_id*			   bone_buffers;
		};

		void init(const init_params& params);
		void uninit();

		void upload(world* w, buffer_queue* queue, uint8 data_index, uint8 frame_index);
		void populate_render_data(world* w, const view& v, const world_render_data& wd, uint8 data_index);
		void render(uint8 data_index, uint8 frame_index, const vector2ui16& size, gfx_id global_layout, gfx_id global_group);
		void resize(const vector2ui16& size);

		inline gfx_id get_color_texture(uint8 frame_index, uint8 texture_index) const
		{
			return _pfd[frame_index].color_textures[texture_index];
		}

		inline gfx_id get_depth_texture(uint8 frame_index) const
		{
			return _pfd[frame_index].depth_texture;
		}

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

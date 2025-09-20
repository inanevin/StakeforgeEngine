// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/bitmask.hpp"
#include "data/static_vector.hpp"
#include "gfx/common/gfx_constants.hpp"
#include "gfx/world/draws.hpp"

namespace SFG
{
	class vector2ui16;
	class bump_allocator;

#define MAX_RP_TEXTURES 4

	class render_pass
	{
	public:
		enum render_pass_flags
		{
			owns_color = 1 << 0,
			owns_depth = 1 << 1,
		};

		struct per_frame_data
		{
			static_vector<gfx_id, MAX_RP_TEXTURES> textures;
			gfx_id								   depth_texture = 0;
			gfx_id								   bind_group	 = 0;
		};

	public:
		void create_color_targets(uint8 target_count, uint8 format, const vector2ui16& size);
		void create_depth_target(uint8 format, const vector2ui16& size);
		void set_color_targets(uint8 target_count, gfx_id* targets);
		void set_depth_target(gfx_id* targets);
		void set_bind_group(uint8 frame_index, gfx_id group);
		void uninit();

		void render(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size);
		void render_w_depth(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size);
		void resize(const vector2ui16& size);

		inline gfx_id get_color_texture(uint8 frame_index, uint8 texture_index) const
		{
			return _pfd[frame_index].textures[texture_index];
		}

		inline gfx_id get_depth_texture(uint8 frame_index) const
		{
			return _pfd[frame_index].depth_texture;
		}

	private:
		void draw(indexed_draw* draws, uint32 draws_count, bump_allocator& alloc, gfx_id cmd_buffer, uint8 frame_index, const vector2ui16& size);

	private:
		per_frame_data _pfd[FRAMES_IN_FLIGHT] = {};
		bitmask<uint8> _flags				  = 0;
		uint8		   _texture_format		  = 0;
		uint8		   _depth_format		  = 0;
	};

}
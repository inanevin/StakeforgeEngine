// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace LinaGX
{
	struct TextureBarrier;
}

namespace Game
{
	class gfx_util
	{
	public:
		constexpr static unsigned int FRAMES_IN_FLIGHT = 2;

		static void init();
		static void uninit();
		static void get_texture_barrier_undef_2_trasfer_dest(uint32 hw, LinaGX::TextureBarrier& out_barrier);
		static void get_texture_barrier_transfer_dest_2_sampled(uint32 hw, LinaGX::TextureBarrier& out_barrier);
		static void get_texture_barrier_color_att_2_present(uint32 hw, LinaGX::TextureBarrier& out_barrier);
		static void get_texture_barrier_present_2_color_att(uint32 hw, LinaGX::TextureBarrier& out_barrier);
	};
}
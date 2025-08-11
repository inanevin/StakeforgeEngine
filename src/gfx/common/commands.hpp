// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/resource_limits.hpp"
#include "math/vector4.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/barrier_description.hpp"

namespace SFG
{
	struct texture_buffer;

	enum render_pass_op_flags
	{
		rpa_load_op_clear	   = 1 << 0,
		rpa_load_op_load	   = 1 << 1,
		rpa_load_op_dont_care  = 1 << 2,
		rpa_store_op_clear	   = 1 << 3,
		rpa_store_op_load	   = 1 << 4,
		rpa_store_op_dont_care = 1 << 5,
	};

	enum render_pass_att_flags
	{
		rpt_is_swapchain = 1 << 0,
		rpt_use_depth	 = 1 << 1,
		rpt_use_stencil	 = 1 << 2,
	};

	enum resource_state_flags
	{
		rsf_transfer_read  = 1 << 0,
		rsf_transfer_write = 1 << 1,
	};

	enum texture_state_flags
	{
		tsf_color_att	  = 1 << 0,
		tsf_depth_att	  = 1 << 1,
		tsf_shader_read	  = 1 << 2,
		tsf_depth_read	  = 1 << 3,
		tsf_stencil_read  = 1 << 4,
		tsf_present		  = 1 << 5,
		tsf_transfer_src  = 1 << 6,
		tsf_transfer_dest = 1 << 7,
	};

	enum render_pass_resolve_flags
	{
		rpa_resolve_mode_min = 1 << 0,
		rpa_resolve_mode_avg = 1 << 1,
	};

	struct render_pass_color_attachment
	{
		vector4		clear_color = vector4(0, 0, 0, 1);
		resource_id texture		= 0;
		load_op		load_op		= load_op::clear;
		store_op	store_op	= store_op::store;
		uint8		view_index	= 0;
	};

	struct render_pass_depth_stencil_attachment
	{
		resource_id	   texture			= 0;
		uint8		   clear_stencil	= 0;
		float		   clear_depth		= 1.0f;
		bitmask<uint8> att_flags		= 0;
		load_op		   depth_load_op	= load_op::clear;
		load_op		   stencil_load_op	= load_op::clear;
		store_op	   depth_store_op	= store_op::dont_care;
		store_op	   stencil_store_op = store_op::dont_care;
		uint8		   view_index		= 0;
	};

	struct command_begin_render_pass
	{
		static constexpr uint8 TID = 0;

		render_pass_color_attachment* color_attachments		 = {};
		uint8						  color_attachment_count = 0;
	};

	struct command_begin_render_pass_depth
	{
		static constexpr uint8 TID = 19;

		render_pass_color_attachment*		 color_attachments		  = {};
		render_pass_depth_stencil_attachment depth_stencil_attachment = {};
		uint8								 color_attachment_count	  = 0;
	};

	struct command_begin_render_pass_swapchain
	{
		static constexpr uint8 TID = 20;

		render_pass_color_attachment* color_attachments		 = {};
		uint8						  color_attachment_count = 0;
	};

	struct command_begin_render_pass_swapchain_depth
	{
		static constexpr uint8 TID = 21;

		render_pass_color_attachment*		 color_attachments		  = {};
		render_pass_depth_stencil_attachment depth_stencil_attachment = {};
		uint8								 color_attachment_count	  = 0;
	};

	struct command_end_render_pass
	{
		static constexpr uint8 TID = 1;
	};

	struct command_set_viewport
	{
		static constexpr uint8 TID = 2;

		float  x		 = 0.0f;
		float  y		 = 0.0f;
		float  min_depth = 0.0f;
		float  max_depth = 0.0f;
		uint16 width	 = 0;
		uint16 height	 = 0;
	};

	struct command_set_scissors
	{
		static constexpr uint8 TID = 3;

		uint16 x	  = 0;
		uint16 y	  = 0;
		uint16 width  = 0;
		uint16 height = 0;
	};

	struct command_bind_pipeline
	{
		static constexpr uint8 TID = 4;

		resource_id pipeline = 0;
	};

	struct command_bind_pipeline_compute
	{
		static constexpr uint8 TID = 22;

		resource_id pipeline = 0;
	};

	struct command_draw_instanced
	{
		static constexpr uint8 TID = 5;

		uint32 vertex_count_per_instance = 0;
		uint32 instance_count			 = 0;
		uint32 start_vertex_location	 = 0;
		uint32 start_instance_location	 = 0;
	};

	struct command_draw_indexed_instanced
	{
		static constexpr uint8 TID = 6;

		uint32 index_count_per_instance = 0;
		uint32 instance_count			= 0;
		uint32 start_index_location		= 0;
		uint32 base_vertex_location		= 0;
		uint32 start_instance_location	= 0;
	};

	struct command_draw_indexed_indirect
	{
		static constexpr uint8 TID = 7;

		resource_id indirect_buffer		   = 0;
		uint32		indirect_buffer_offset = 0;
		uint16		count				   = 0;
		resource_id indirect_signature	   = 0;
	};

	struct command_draw_indirect
	{
		static constexpr uint8 TID = 8;

		resource_id indirect_buffer		   = 0;
		uint32		indirect_buffer_offset = 0;
		uint16		count				   = 0;
		resource_id indirect_signature	   = 0;
	};

	struct command_copy_resource
	{
		static constexpr uint8 TID = 9;

		resource_id source		= 0;
		resource_id destination = 0;
	};

	struct command_copy_texture_to_buffer
	{
		static constexpr uint8 TID = 10;

		resource_id dest_buffer = 0;
		resource_id src_texture = 0;
		uint32		src_layer	= 0;
		uint32		src_mip		= 0;
		vector2ui	size		= vector2ui::zero;
		uint8		bpp			= 0;
	};

	struct command_copy_buffer_to_texture
	{
		static constexpr uint8 TID = 11;

		texture_buffer* textures			= nullptr;
		resource_id		destination_texture = 0;
		resource_id		intermediate_buffer = 0;
		uint8			mip_levels			= 0;
		uint8			destination_slice	= 0;
	};

	struct command_copy_texture_to_texture
	{
		static constexpr uint8 TID = 12;

		resource_id source				   = 0;
		resource_id destination			   = 0;
		uint8		source_layer		   = 0;
		uint8		destination_layer	   = 0;
		uint8		source_mip			   = 0;
		uint8		source_total_mips	   = 0;
		uint8		destination_mip		   = 0;
		uint8		destination_total_mips = 0;
	};

	struct command_bind_vertex_buffers
	{
		static constexpr uint8 TID = 13;

		resource_id buffer		= 0;
		uint8		slot		= 0;
		uint16		vertex_size = 0;
		uint64		offset		= 0;
	};

	struct command_bind_index_buffers
	{
		static constexpr uint8 TID = 14;

		resource_id buffer	  = 0;
		uint64		offset	  = 0;
		uint8		bit_depth = 0;
	};

	struct command_bind_group
	{
		static constexpr uint8 TID = 15;

		resource_id group = 0;
	};

	struct command_bind_constants
	{
		static constexpr uint8 TID = 16;

		void*  data		   = nullptr;
		uint16 offset	   = 0;
		uint8  count	   = 0;
		uint8  param_index = 0;
	};

	struct command_dispatch
	{
		static constexpr uint8 TID = 17;

		uint32 group_size_x = 0;
		uint32 group_size_y = 0;
		uint32 group_size_z = 0;
	};

	struct command_barrier
	{
		static constexpr uint8 TID = 18;

		const barrier* barriers		 = nullptr;
		uint16		   barrier_count = 0;
	};

	struct command_bind_layout
	{
		static constexpr uint8 TID = 23;

		resource_id layout = 0;
	};

	struct command_bind_layout_compute
	{
		static constexpr uint8 TID = 24;

		resource_id layout = 0;
	};

}
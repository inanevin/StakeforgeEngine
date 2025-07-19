// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/common/gfx_common.hpp"
#include "math/vector4.hpp"
#include "data/bitmask.hpp"

namespace Game
{

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

	struct texture_barrier
	{
		resource_id	   texture		= 0;
		bitmask<uint8> state_flags	= 0;
		uint8		   is_swapchain = 0;
	};

	struct resource_barrier
	{
		resource_id	   resource	   = 0;
		bitmask<uint8> state_flags = 0;
	};

	struct render_pass_color_attachment
	{
		vector4		   clear_color		   = vector4(0, 0, 0, 1);
		resource_id	   texture			   = 0;
		resource_id	   resolve_texture	   = 0;
		bitmask<uint8> resolve_flags	   = 0;
		bitmask<uint8> resolve_state_flags = 0;
		bitmask<uint8> op_flags			   = 0;
		bitmask<uint8> att_flags		   = 0;
		uint8		   view_index		   = 0;
		uint8		   resolve_view_index  = 0;
	};

	struct render_pass_depth_stencil_attachment
	{
		resource_id	   texture			   = 0;
		resource_id	   resolve_texture	   = 0;
		uint32		   clear_stencil	   = 0;
		float		   clear_depth		   = 1.0f;
		bitmask<uint8> resolve_flags	   = 0;
		bitmask<uint8> resolve_state_flags = 0;
		bitmask<uint8> att_flags		   = 0;
		bitmask<uint8> depth_op_flags	   = 0;
		bitmask<uint8> stencil_op_flags	   = 0;
		uint8		   view_index		   = 0;
		uint8		   resolve_view_index  = 0;
	};

	struct command_begin_render_pass
	{
		uint16 color_attachments	  = 0;
		uint16 depth_attachment		  = 0;
		uint16 viewport				  = 0;
		uint16 scissors				  = 0;
		uint8  color_attachment_count = 0;
	};

	struct command_end_render_pass
	{
	};

	struct command_set_viewport
	{
		float  x		 = 0.0f;
		float  y		 = 0.0f;
		float  min_depth = 0.0f;
		float  max_depth = 0.0f;
		uint16 width	 = 0;
		uint16 height	 = 0;
	};

	struct command_set_scissors
	{
		uint32 x	  = 0;
		uint32 y	  = 0;
		uint16 width  = 0;
		uint16 height = 0;
	};

	struct command_bind_pipeline
	{
		resource_id pipeline = 0;
	};

	struct command_draw_instanced
	{
		uint32 vertex_count_per_instance = 0;
		uint32 instance_count			 = 0;
		uint32 start_vertex_location	 = 0;
		uint32 start_instance_location	 = 0;
	};

	struct command_draw_indexed_instanced
	{
		uint32 index_count_per_instance = 0;
		uint32 instance_count			= 0;
		uint32 start_index_location		= 0;
		uint32 base_vertex_location		= 0;
		uint32 start_instance_location	= 0;
	};

	struct command_draw_indexed_indirect
	{
		resource_id indirect_buffer		   = 0;
		uint32		indirect_buffer_offset = 0;
		uint16		count				   = 0;
	};

	struct command_draw_indirect
	{
		resource_id indirect_buffer		   = 0;
		uint32		indirect_buffer_offset = 0;
		uint16		count				   = 0;
	};

	struct command_copy_resource
	{
		resource_id source		= 0;
		resource_id destination = 0;
	};

	struct command_copy_buffer_to_texture2d
	{
		resource_id		destination_texture = 0;
		uint32			mip_levels			= 0;
		uint16			destination_slice	= 0;
		texture_buffer* textures			= nullptr;
	};

	struct command_copy_texture2d_o_texture2d
	{
		resource_id source			  = 0;
		resource_id destination		  = 0;
		uint8		source_layer	  = 0;
		uint8		destination_layer = 0;
		uint8		source_mip		  = 0;
		uint8		destination_mip	  = 0;
	};

	struct command_bind_vertex_buffers
	{
		resource_id buffer		= 0;
		uint8		slot		= 0;
		uint16		vertex_size = 0;
		uint64		offset		= 0;
	};

	struct command_bind_index_buffers
	{
		resource_id buffer	  = 0;
		uint64		offset	  = 0;
		uint8		bit_depth = 0;
	};

	struct command_bind_group
	{
		resource_id group = 0;
	};

	struct command_bind_constants
	{
		void*  data	  = nullptr;
		uint16 offset = 0;
		uint16 size	  = 0;
	};

	struct command_dispatch
	{
		uint32 group_size_x = 0;
		uint32 group_size_y = 0;
		uint32 group_size_z = 0;
	};

	struct command_barrier
	{
		uint16 texture_barriers		  = 0;
		uint16 resource_barriers	  = 0;
		uint16 texture_barrier_count  = 0;
		uint16 resource_barrier_count = 0;
	};

}
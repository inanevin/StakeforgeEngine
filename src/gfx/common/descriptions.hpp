// Copyright (c) 2025 Inan Evin
#pragma once

#include "math/vector2.hpp"
#include "math/vector2ui.hpp"
#include "math/vector2ui16.hpp"
#include "data/bitmask.hpp"
#include "data/vector.hpp"
#include "data/span.hpp"
#include "format.hpp"
#include "resource_limits.hpp"
#include "shader_description.hpp"

namespace Game
{
	struct viewport
	{
		vector2		pos		 = vector2::zero;
		vector2ui16 size	 = vector2ui16::zero;
		float		minDepth = 0.0f;
		float		maxDepth = 1.0f;
	};

	struct scissors_rect
	{
		vector2ui16 pos	 = vector2ui16::zero;
		vector2ui16 size = vector2ui16::zero;
	};

	enum class command_type : uint8
	{
		graphics,
		transfer,
		compute,
	};

	enum resource_flags
	{
		rf_vertex_buffer   = 1 << 0,
		rf_index_buffer	   = 1 << 1,
		rf_constant_buffer = 1 << 2,
		rf_storage_buffer  = 1 << 3,
		rf_indirect_buffer = 1 << 4,
		rf_gpu_only		   = 1 << 5,
		rf_cpu_visible	   = 1 << 6,
		rf_gpu_write	   = 1 << 7,
	};

	enum swapchain_flags
	{
		sf_is_full_screen		= 1 << 0,
		sf_vsync_every_v_blank	= 1 << 1,
		sf_vsync_every_2v_blank = 1 << 2,
		sf_allow_tearing		= 1 << 3,
	};

	enum binding_type : uint8
	{
		constant,
		ubo,
		ssbo,
		uav,
		pointer,
		sampler,
		texture,
	};

	enum binding_flags
	{
		bf_ubo		 = 1 << 0,
		bf_ssbo		 = 1 << 1,
		bf_uav		 = 1 << 2,
		bf_constant	 = 1 << 3,
		bf_table	 = 1 << 4,
		bf_unbounded = 1 << 5,
		bf_sampler	 = 1 << 6,
		bf_texture	 = 1 << 7,
	};

	enum texture_flags
	{
		tf_render_target			= 1 << 0,
		tf_depth_texture			= 1 << 1,
		tf_stencil_texture			= 1 << 2,
		tf_linear_tiling			= 1 << 3,
		tf_sampled					= 1 << 4,
		tf_sampled_outside_fragment = 1 << 5,
		tf_transfer_source			= 1 << 6,
		tf_transfer_dest			= 1 << 7,
		tf_cubemap					= 1 << 8,
		tf_readback					= 1 << 9,
		tf_is_2d					= 1 << 10,
		tf_is_3d					= 1 << 11,
		tf_is_1d					= 1 << 12,
	};

	enum sampler_flags
	{
		saf_min_anisotropic				 = 1 << 0,
		saf_min_nearest					 = 1 << 1,
		saf_min_linear					 = 1 << 2,
		saf_mag_anisotropic				 = 1 << 3,
		saf_mag_nearest					 = 1 << 4,
		saf_mag_linear					 = 1 << 5,
		saf_mip_nearest					 = 1 << 6,
		saf_mip_linear					 = 1 << 7,
		saf_address_mode_repeat			 = 1 << 8,
		saf_address_mode_border			 = 1 << 9,
		saf_address_mode_clamp			 = 1 << 10,
		saf_address_mode_mirrored_repeat = 1 << 11,
		saf_address_mode_mirrored_clamp	 = 1 << 12,
		saf_border_transparent			 = 1 << 13,
		saf_border_white				 = 1 << 14,
	};

	struct swapchain_desc
	{
		void*		   window	 = nullptr;
		void*		   os_handle = nullptr;
		float		   scaling	 = 1.0f;
		format		   format	 = format::undefined;
		vector2ui16	   pos		 = vector2ui16::zero;
		vector2ui16	   size		 = vector2ui16::zero;
		bitmask<uint8> flags	 = 0;
	};

	struct swapchain_recreate_desc
	{
		vector2ui16	   size		 = vector2ui16::zero;
		resource_id	   swapchain = 0;
		float		   scaling	 = 1.0f;
		bitmask<uint8> flags	 = 0;
	};

	struct resource_desc
	{
		uint32		   size			  = 0;
		bitmask<uint8> flags		  = 0;
		char		   debug_name[16] = {"Resource"};
	};

	struct view_desc
	{
		uint8 base_arr_level = 0;
		uint8 level_count	 = 1;
		uint8 base_mip_level = 0;
		uint8 mip_count		 = 1;
		uint8 is_cubemap	 = 0;
	};

	struct texture_desc
	{
		format			  texture_format	   = format::r8g8b8a8_srgb;
		format			  depth_stencil_format = format::r32_sfloat;
		vector2ui16		  size				   = vector2ui16::zero;
		bitmask<uint16>	  flags				   = 0;
		vector<view_desc> views				   = {
			   {},
		   };
		uint8 mip_levels	 = 1;
		uint8 array_length	 = 1;
		uint8 samples		 = 1;
		char  debug_name[16] = {"Texture"};
	};

	struct sampler_desc
	{
		uint32			anisotropy	   = 0;
		float			min_lod		   = 0.0f;
		float			max_lod		   = 1.0f;
		float			lod_bias	   = 0.0f;
		bitmask<uint16> flags		   = 0;
		char			debug_name[16] = {"Sampler"};
	};

	struct layout_entry
	{
		binding_type type					= binding_type::constant;
		uint8		 count					= 1;
		uint8		 set					= 0;
		uint8		 binding				= 0;
		sampler_desc immutable_sampler_desc = {};
	};

	struct binding
	{
		vector<layout_entry> entry_table;
		shader_stage		 visibility;
	};

	struct bind_layout_pointer_param
	{
		binding_type type		 = binding_type::ubo;
		uint8		 set		 = 0;
		uint8		 binding	 = 0;
		uint8		 count		 = 0;
		uint8		 is_volatile = 0;
	};

	struct bind_group_pointer
	{
		resource_id	 resource	   = 0;
		uint8		 view		   = 0;
		uint8		 pointer_index = 0;
		binding_type type		   = binding_type::ubo;
	};

	struct bind_group_binding
	{
		uint8*		 constants	= nullptr;
		uint8		 root_index = 0;
		uint8		 count		= 0;
		binding_type type		= binding_type::constant;
	};

	struct binding_update
	{
		uint32				 binding_index	= 0;
		vector<binding_type> resource_types = {};
		vector<resource_id>	 resources		= {};
		vector<uint32>		 resource_views = {};
	};

	struct bind_group_update_desc
	{
		vector<binding_update> updates;
	};

	struct queue_desc
	{
		command_type type			= command_type::graphics;
		char		 debug_name[16] = {"Debug Name"};
	};

	struct command_buffer_desc
	{
		command_type type			= command_type::graphics;
		char		 debug_name[16] = {"CmdBuffer"};
	};

}
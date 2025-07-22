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

	enum class blend_op : uint8
	{
		add,
		subtract,
		reverse_subtract,
		min,
		max
	};

	enum class blend_factor : uint16
	{
		zero,
		one,
		src_color,
		one_minus_src_color,
		dst_color,
		one_minus_dst_color,
		src_alpha,
		one_minus_src_alpha,
		dst_alpha,
		one_minus_dst_alpha,
	};

	enum class logic_op : uint16
	{
		clear,
		and_,
		and_reverse,
		copy,
		and_inverted,
		no_op,
		xor_,
		or_,
		nor,
		equivalent,
	};

	enum class command_type : uint8
	{
		graphics,
		transfer,
		compute,
	};

	enum class topology : uint8
	{
		point_list,
		line_list,
		line_strip,
		triangle_list,
		triangle_strip,
		triangle_fan,
		triangle_list_adjacency,
		triangle_strip_adjacency,
	};

	enum class polygon_mode : uint8
	{
		fill,
		line,
		point
	};

	enum class cull_mode : uint8
	{
		none,
		front,
		back,
	};

	enum class front_face : uint8
	{
		ccw,
		cw,
	};

	enum class stencil_op : uint8
	{
		keep,
		zero,
		replace,
		increment_clamp,
		decrement_clamp,
		invert,
		increment_wrap,
		decrement_wrap,
	};

	enum class load_op : uint8
	{
		load,
		clear,
		dont_care,
		none,
	};

	enum class compare_op : uint8
	{
		never,
		less,
		equal,
		lequal,
		greater,
		nequal,
		gequal,
		always
	};

	enum class store_op : uint8
	{
		store,
		dont_care,
		none,
	};

	enum class shader_stage : uint8
	{
		vertex,
		fragment,
		compute,
		all
	};

	enum class resource_state : uint8
	{
		common,
		vertex_cbv,
		index_buffer,
		render_target,
		uav,
		depth_write,
		depth_read,
		non_ps_resource,
		ps_resource,
		indirect_arg,
		copy_dest,
		copy_source,
		resolve_dest,
		resolve_source,
		generic_read,
		present,
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

	enum class descriptor_type : uint8
	{
		constant,
		ubo,
		ssbo,
		uav,
		pointer,
		sampler,
		texture
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
		tf_color_attachment			= 1 << 0,
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

	enum shader_flags
	{
		shf_enable_sample_shading = 1 << 0,
		shf_enable_depth_bias	  = 1 << 1,
		shf_enable_alpha_to_cov	  = 1 << 2,
		shf_enable_blend_logic_op = 1 << 3,
		shf_use_embedded_layout	  = 1 << 4,
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
		vector2ui	   pos		 = vector2ui::zero;
		vector2ui	   size		 = vector2ui::zero;
		bitmask<uint8> flags	 = 0;
	};

	struct swapchain_recreate_desc
	{
		vector2ui	   size		 = vector2ui::zero;
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
		uint8 level_count	 = 0;
		uint8 base_mip_level = 0;
		uint8 mip_count		 = 0;
		uint8 is_cubemap	 = 0;
	};

	struct texture_desc
	{
		format			  texture_format	   = format::r8g8b8a8_srgb;
		format			  depth_stencil_format = format::r32_sfloat;
		vector2ui16		  size				   = vector2ui16::zero;
		bitmask<uint16>	  flags				   = 0;
		vector<view_desc> views				   = {};
		uint8			  mip_levels		   = 1;
		uint8			  array_length		   = 1;
		uint8			  samples			   = 1;
		char			  debug_name[16]	   = {"Texture"};
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
		descriptor_type type				   = descriptor_type::constant;
		uint8			count				   = 1;
		uint8			set					   = 0;
		uint8			binding				   = 0;
		sampler_desc	immutable_sampler_desc = {};
	};

	struct binding
	{
		vector<layout_entry> entry_table;
		shader_stage		 visibility = shader_stage::vertex;
	};

	struct bind_layout_desc
	{
		vector<binding> bindings   = {};
		uint8			is_compute = 0;
	};

	struct bind_group_binding
	{
		uint32			root_index		= 0;
		uint32			count			= 0;
		descriptor_type type			= descriptor_type::constant;
		uint32			constant_value	= 0;
		uint32			constant_offset = 0;
	};

	struct bind_group_desc
	{
		vector<bind_group_binding> bindings;
	};

	struct binding_update
	{
		uint32					binding_index  = 0;
		vector<descriptor_type> resource_types = {};
		vector<resource_id>		resources	   = {};
		vector<uint32>			resource_views = {};
	};

	struct bind_group_update_desc
	{
		vector<binding_update> updates;
	};

	struct pipeline_layout_desc
	{
		vector<bind_group_desc> set_descriptions = {};
		uint8					is_compute		 = 0;
		char					debug_name[16]	 = {"PipelineLayout"};
	};

	struct vertex_input
	{
		char   name[16] = "TEXCOORD";
		uint8  location = 0;
		uint8  index	= 0;
		size_t offset	= 0;
		size_t size		= 0;
		format format	= format::undefined;
	};

	struct shader_blob
	{
		shader_stage stage = {};
		span<uint8>	 data  = {};
	};

	enum color_comp_flags
	{
		ccf_red	  = 1 << 0,
		ccf_green = 1 << 1,
		ccf_blue  = 1 << 2,
		ccf_alpha = 1 << 3,
	};

	struct color_blend_attachment
	{
		bool		   blend_enabled		  = false;
		blend_factor   src_color_blend_factor = blend_factor::src_alpha;
		blend_factor   dst_color_blend_factor = blend_factor::one_minus_src_alpha;
		blend_op	   color_blend_op		  = blend_op::add;
		blend_factor   src_alpha_blend_factor = blend_factor::one;
		blend_factor   dst_alpha_blend_factor = blend_factor::zero;
		blend_op	   alpha_blend_op		  = blend_op::add;
		bitmask<uint8> color_comp_flags		  = ccf_red | ccf_green | ccf_blue | ccf_alpha;
	};

	struct shader_color_attachment
	{
		format				   format			= format::b8g8r8a8_srgb;
		color_blend_attachment blend_attachment = {};
	};

	enum depth_stencil_flags
	{
		dsf_depth_write	   = 1 << 0,
		dsf_depth_test	   = 1 << 1,
		dsf_enable_stencil = 1 << 2,
	};
	struct stencil_state
	{
		stencil_op fail_op		 = stencil_op::keep;
		stencil_op pass_op		 = stencil_op::keep;
		stencil_op depth_fail_op = stencil_op::keep;
		compare_op compare_op	 = compare_op::always;
	};

	struct shader_depth_stencil_desc
	{
		format		   attachment_format	= format::d32_sfloat;
		compare_op	   depth_compare		= compare_op::lequal;
		stencil_state  back_stencil_state	= {};
		stencil_state  front_stencil_state	= {};
		uint32		   stencil_compare_mask = 0xFF;
		uint32		   stencil_write_mask	= 0xFF;
		bitmask<uint8> flags				= 0;
	};

	struct shader_desc
	{
		const char*						vertex_entry  = "VSMain";
		const char*						pixel_entry	  = "PSMain";
		const char*						compute_entry = "CSMain";
		bitmask<uint16>					flags		  = 0;
		span<uint8>						layout_data	  = {};
		vector<shader_blob>				blobs		  = {};
		vector<shader_color_attachment> attachments	  = {};
		vector<vertex_input>			inputs		  = {};

		shader_depth_stencil_desc depth_stencil_desc = {};
		logic_op				  blend_logic_op	 = logic_op::and_;
		topology				  topo				 = topology::triangle_list;
		cull_mode				  cull				 = cull_mode::back;
		front_face				  front				 = front_face::cw;
		polygon_mode			  poly_mode			 = polygon_mode::fill;
		resource_id				  layout			 = 0;

		uint32 samples			   = 1;
		float  depth_bias_constant = 0.0f;
		float  depth_bias_clamp	   = 0.0f;
		float  depth_bias_slope	   = 0.0f;

		char debug_name[16] = {"Shader"};
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
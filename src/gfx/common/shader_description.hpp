// Copyright (c) 2025 Inan Evin
#pragma once

#include "data/bitmask.hpp"
#include "data/vector.hpp"
#include "data/span.hpp"
#include "format.hpp"
#include "resource_limits.hpp"

namespace Game
{
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

	enum class topology : uint8
	{
		point_list,
		line_list,
		line_strip,
		triangle_list,
		triangle_strip,
		triangle_fan,
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

	enum shader_stage : uint8
	{
		vertex,
		fragment,
		compute,
		all
	};

	enum shader_flags
	{
		shf_enable_sample_shading = 1 << 0,
		shf_enable_depth_bias	  = 1 << 1,
		shf_enable_alpha_to_cov	  = 1 << 2,
		shf_enable_blend_logic_op = 1 << 3,
		shf_use_embedded_layout	  = 1 << 4,
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

		char debug_name[16] = "shader";

		inline void set_name(const char* name)
		{
			strcpy(debug_name, name);
		}
	};

}
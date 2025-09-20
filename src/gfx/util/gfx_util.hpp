// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"

namespace SFG
{
	enum class input_layout_type
	{
		gui_default,
	};

	enum root_param_index : uint8
	{
		rpi_constants = 0,
		rpi_engine_cbv,
		rpi_table_render_pass,
		rpi_table_material,
		rpi_table_object,
		rpi_table_dyn_sampler,
		rpi_static_sampler_begin,
	};

	enum update_pointer_index : uint8
	{
		upi_render_pass_ubo0	 = 0,
		upi_render_pass_ubo1	 = 1,
		upi_render_pass_ssbo0	 = 2,
		upi_render_pass_ssbo1	 = 3,
		upi_render_pass_ssbo2	 = 4,
		upi_render_pass_ssbo3	 = 5,
		upi_render_pass_texture0 = 6,
		upi_render_pass_texture1 = 7,
		upi_render_pass_texture2 = 8,
		upi_render_pass_texture3 = 9,

		upi_material_ubo0	  = 0,
		upi_material_ssbo0	  = 1,
		upi_material_texture0 = 2,
		upi_material_texture1 = 3,
		upi_material_texture2 = 4,
		upi_material_texture3 = 5,

		upi_object_ubo0		= 0,
		upi_object_texture0 = 1,

		upi_dyn_sampler0 = 0,
		upi_dyn_sampler1 = 1,
		upi_dyn_sampler2 = 2,
		upi_dyn_sampler3 = 3,
	};

	class gfx_util
	{
	public:
		static gfx_id				  create_bind_layout_global();
		static void					  update_dummy_bind_group(gfx_id group, gfx_id dummy_texture, gfx_id dummy_sampler, gfx_id dummy_ssbo, gfx_id dummy_ubo);
		static gfx_id				  create_bind_group_global();
		static sampler_desc			  get_sampler_desc_anisotropic();
		static sampler_desc			  get_sampler_desc_linear();
		static sampler_desc			  get_sampler_desc_nearest();
		static sampler_desc			  get_sampler_desc_gui_default();
		static sampler_desc			  get_sampler_desc_gui_text();
		static color_blend_attachment get_blend_attachment_alpha_blending();
		static vector<vertex_input>	  get_input_layout(input_layout_type type);
	};
}
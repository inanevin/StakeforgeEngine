// Copyright (c) 2025 Inan Evin
#include "gfx_util.hpp"
#include "math/vector4.hpp"
#include "gfx/backend/backend.hpp"

namespace Game
{

	/*
		Global Layout Definition:

		// Param 0 - Root CBV, Global Data [b0, space0]
		// Param 1 - Descriptor Table, Render Pass Data: 2x CBV + 4 SRVs + 4 Textures [b0, b1, space1] - [t0, t3, space1] - [t4, t7, space1]
		// Param 2 - Descriptor Table, Material Data: CBV + SSBO + 4 textures, [b0, space2] - [t0, space2] - [t1, t4, space2]
		// Param 3 - Descriptor Table, Object Data: CBV x 2 + Texture, [b0, b1, space3] - [t0, space3]
		// Param 4 - Descriptor Table, Dynamic Samplers: [s0, s4, space0]
		// Param 5 - Immutable Sampler, Anisotropic, s5, space0
		// Param 6 - Immutable Sampler, Linear, s6, space0
		// Param 7 - Immutable Sampler, Nearest, s7, space0
		// Param 8 - Immutable Sampler, GUI Default, s8, space0
		// Param 9 - Immutable Sampler, GUI Text, s9, space0
	*/

	resource_id gfx_util::create_bind_layout_global()
	{
		gfx_backend* backend = gfx_backend::get();

		resource_id layout = backend->create_empty_bind_layout();

		// Param 0 - Global CBV, [b0, space0]
		backend->bind_layout_add_descriptor(layout, binding_type::ubo, 0, 0, shader_stage::all);

		// Param 1 - Render Pass Table -> 2x CBV + 4 SRVs + 4 Textures [b0, b1, space1] - [t0, t3, space1] - [t4, t7, space1]
		backend->bind_layout_add_pointer(layout,
										 {
											 {.type = binding_type::ubo, .set = 1, .binding = 0, .count = 2, .is_volatile = 1},
											 {.type = binding_type::ssbo, .set = 1, .binding = 0, .count = 4, .is_volatile = 1},
											 {.type = binding_type::texture, .set = 1, .binding = 4, .count = 4, .is_volatile = 1},
										 },
										 shader_stage::all);

		// Param 2 - Material Table -> CBV + SSBO + 4 textures, [b0, space2] - [t0, space2] - [t1, t4, space2]
		backend->bind_layout_add_pointer(layout,
										 {
											 {.type = binding_type::ubo, .set = 2, .binding = 0, .count = 1, .is_volatile = 1},
											 {.type = binding_type::ssbo, .set = 2, .binding = 0, .count = 1, .is_volatile = 1},
											 {.type = binding_type::texture, .set = 2, .binding = 1, .count = 4, .is_volatile = 1},
										 },
										 shader_stage::all);

		// Param 3 - Object Table -> CBV x 2 + Texture, [b0, b1, space3] - [t0, space3]
		backend->bind_layout_add_pointer(layout,
										 {
											 {.type = binding_type::ubo, .set = 3, .binding = 0, .count = 1, .is_volatile = 1},
											 {.type = binding_type::texture, .set = 3, .binding = 0, .count = 1, .is_volatile = 1},
										 },
										 shader_stage::all);

		// Param 4 - 4x Dynamic Samplers, [s0, s4, space0]
		backend->bind_layout_add_pointer(layout,
										 {
											 {.type = binding_type::sampler, .set = 0, .binding = 0, .count = 4},
										 },
										 shader_stage::fragment);

		// Param [5,9] - Immutable Samplers [s5, s9, space0]
		backend->bind_layout_add_immutable_sampler(layout, 0, 5, gfx_util::get_sampler_desc_anisotropic(), shader_stage::fragment);
		backend->bind_layout_add_immutable_sampler(layout, 0, 6, gfx_util::get_sampler_desc_linear(), shader_stage::fragment);
		backend->bind_layout_add_immutable_sampler(layout, 0, 7, gfx_util::get_sampler_desc_nearest(), shader_stage::fragment);
		backend->bind_layout_add_immutable_sampler(layout, 0, 8, gfx_util::get_sampler_desc_gui_default(), shader_stage::fragment);
		backend->bind_layout_add_immutable_sampler(layout, 0, 9, gfx_util::get_sampler_desc_gui_text(), shader_stage::fragment);

		backend->finalize_bind_layout(layout, false, "global_layout");

		return layout;
	}

	void gfx_util::update_dummy_bind_group(resource_id group, resource_id dummy_texture, resource_id dummy_sampler, resource_id dummy_ssbo, resource_id dummy_ubo)
	{
		gfx_backend* backend = gfx_backend::get();

		// Render pass
		backend->bind_group_update_pointer(group,
										   1,
										   {
											   {.resource = dummy_ubo, .pointer_index = upi_render_pass_ubo0, .type = binding_type::ubo},
											   {.resource = dummy_ubo, .pointer_index = upi_render_pass_ubo1, .type = binding_type::ubo},
											   {.resource = dummy_ssbo, .pointer_index = upi_render_pass_ssbo0, .type = binding_type::ssbo},
											   {.resource = dummy_ssbo, .pointer_index = upi_render_pass_ssbo1, .type = binding_type::ssbo},
											   {.resource = dummy_ssbo, .pointer_index = upi_render_pass_ssbo2, .type = binding_type::ssbo},
											   {.resource = dummy_ssbo, .pointer_index = upi_render_pass_ssbo3, .type = binding_type::ssbo},
											   {.resource = dummy_texture, .pointer_index = upi_render_pass_texture0, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_render_pass_texture1, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_render_pass_texture2, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_render_pass_texture3, .type = binding_type::texture},
										   });

		// Materials
		backend->bind_group_update_pointer(group,
										   2,
										   {
											   {.resource = dummy_ubo, .pointer_index = upi_material_ubo0, .type = binding_type::ubo},
											   {.resource = dummy_ssbo, .pointer_index = upi_material_ssbo0, .type = binding_type::ssbo},
											   {.resource = dummy_texture, .pointer_index = upi_material_texture0, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_material_texture1, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_material_texture2, .type = binding_type::texture},
											   {.resource = dummy_texture, .pointer_index = upi_material_texture3, .type = binding_type::texture},
										   });

		// Objects
		backend->bind_group_update_pointer(group,
										   3,
										   {
											   {.resource = dummy_ubo, .pointer_index = upi_object_ubo0, .type = binding_type::ubo},
											   {.resource = dummy_texture, .pointer_index = upi_object_texture0, .type = binding_type::texture},
										   });

		// Dynamic samplers
		backend->bind_group_update_pointer(group,
										   4,
										   {
											   {.resource = dummy_sampler, .pointer_index = upi_dyn_sampler0, .type = binding_type::sampler},
											   {.resource = dummy_sampler, .pointer_index = upi_dyn_sampler1, .type = binding_type::sampler},
											   {.resource = dummy_sampler, .pointer_index = upi_dyn_sampler2, .type = binding_type::sampler},
											   {.resource = dummy_sampler, .pointer_index = upi_dyn_sampler3, .type = binding_type::sampler},
										   });
	}

	resource_id gfx_util::create_bind_group_global()
	{
		gfx_backend* backend = gfx_backend::get();
		resource_id	 group	 = backend->create_empty_bind_group();

		// global
		backend->bind_group_add_descriptor(group, rpi_engine_cbv, binding_type::ubo);

		// RP - Material - Object - Dynamic Sampler Tables
		backend->bind_group_add_pointer(group, rpi_table_render_pass, 10, false);
		backend->bind_group_add_pointer(group, rpi_table_material, 6, false);
		backend->bind_group_add_pointer(group, rpi_table_object, 3, false);
		backend->bind_group_add_pointer(group, rpi_table_dyn_sampler, 4, true);

		return group;
	}

	sampler_desc gfx_util::get_sampler_desc_anisotropic()
	{
		return {
			.anisotropy = 6,
			.min_lod	= 0.0f,
			.max_lod	= 10.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_anisotropic | sampler_flags::saf_mag_anisotropic | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_linear | sampler_flags::saf_border_transparent,
		};
	}
	sampler_desc gfx_util::get_sampler_desc_linear()
	{
		return {
			.anisotropy = 0,
			.min_lod	= 0.0f,
			.max_lod	= 10.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_linear | sampler_flags::saf_mag_linear | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_linear | sampler_flags::saf_border_transparent,
		};
	}

	sampler_desc gfx_util::get_sampler_desc_nearest()
	{
		return {
			.anisotropy = 0,
			.min_lod	= 0.0f,
			.max_lod	= 10.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_nearest | sampler_flags::saf_mag_nearest | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_nearest | sampler_flags::saf_border_transparent,
		};
	}

	sampler_desc gfx_util::get_sampler_desc_gui_default()
	{
		return {
			.anisotropy = 0,
			.min_lod	= 0.0f,
			.max_lod	= 10.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_linear | sampler_flags::saf_mag_linear | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_linear | sampler_flags::saf_border_transparent,
		};
	}

	sampler_desc gfx_util::get_sampler_desc_gui_text()
	{
		return {
			.anisotropy = 0,
			.min_lod	= 0.0f,
			.max_lod	= 1.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_nearest | sampler_flags::saf_mag_nearest | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_linear | sampler_flags::saf_border_transparent,
		};
	}

	color_blend_attachment gfx_util::get_blend_attachment_alpha_blending()
	{
		return {
			.blend_enabled			= true,
			.src_color_blend_factor = blend_factor::src_alpha,
			.dst_color_blend_factor = blend_factor::one_minus_src_alpha,
			.color_blend_op			= blend_op::add,
			.src_alpha_blend_factor = blend_factor::one,
			.dst_alpha_blend_factor = blend_factor::one_minus_src_alpha,
			.alpha_blend_op			= blend_op::add,
			.color_comp_flags		= ccf_red | ccf_green | ccf_blue | ccf_alpha,
		};
	}

	vector<vertex_input> gfx_util::get_input_layout(input_layout_type type)
	{
		vector<vertex_input> inputs;

		switch (type)
		{
		case input_layout_type::gui_default: {
			inputs = {
				{
					.name	  = "POSITION",
					.location = 0,
					.index	  = 0,
					.offset	  = 0,
					.size	  = sizeof(vector2),
					.format	  = format::r32g32_sfloat,
				},
				{
					.name	  = "TEXCOORD",
					.location = 0,
					.index	  = 0,
					.offset	  = sizeof(vector2),
					.size	  = sizeof(vector2),
					.format	  = format::r32g32_sfloat,
				},
				{
					.name	  = "COLOR",
					.location = 0,
					.index	  = 0,
					.offset	  = sizeof(vector2) * 2,
					.size	  = sizeof(vector4),
					.format	  = format::r32g32b32a32_sfloat,
				},
			};
			break;
		}
		default:
			break;
		}

		return inputs;
	}

}
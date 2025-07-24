// Copyright (c) 2025 Inan Evin
#include "gfx_util.hpp"
#include "math/vector4.hpp"

namespace Game
{
	sampler_desc gfx_util::get_sampler_desc_gui_default()
	{
		return {
			.anisotropy = 0,
			.min_lod	= 0.0f,
			.max_lod	= 1.0f,
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
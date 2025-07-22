// Copyright (c) 2025 Inan Evin
#include "gfx_util.hpp"

namespace Game
{
	sampler_desc gfx_util::get_sampler_desc_gui_default()
	{
		return {
			.anisotropy = 5,
			.min_lod	= 0.0f,
			.max_lod	= 1.0f,
			.lod_bias	= 0.0f,
			.flags		= sampler_flags::saf_min_linear | sampler_flags::saf_mag_linear | sampler_flags::saf_address_mode_clamp | sampler_flags::saf_mip_linear | sampler_flags::saf_border_transparent,
		};
	}

	color_blend_attachment gfx_util::get_blend_attachment_alpha_blending()
	{
		return {
			.blend_enabled			= false,
			.src_color_blend_factor = blend_factor::src_alpha,
			.dst_color_blend_factor = blend_factor::one_minus_src_alpha,
			.color_blend_op			= blend_op::add,
			.src_alpha_blend_factor = blend_factor::one,
			.dst_alpha_blend_factor = blend_factor::zero,
			.alpha_blend_op			= blend_op::add,
			.color_comp_flags		= ccf_red | ccf_green | ccf_blue | ccf_alpha,
		};
	}

}
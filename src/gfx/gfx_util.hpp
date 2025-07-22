// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"

namespace Game
{
	class gfx_util
	{
	public:
		static sampler_desc			  get_sampler_desc_gui_default();
		static color_blend_attachment get_blend_attachment_alpha_blending();
	};
}
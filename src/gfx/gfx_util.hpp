// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "gfx/common/descriptions.hpp"

namespace Game
{
	enum class input_layout_type
	{
		gui_default,
	};

	class gfx_util
	{
	public:
		static sampler_desc			  get_sampler_desc_gui_default();
		static sampler_desc			  get_sampler_desc_gui_text();
		static color_blend_attachment get_blend_attachment_alpha_blending();
		static vector<vertex_input>	  get_input_layout(input_layout_type type);
	};
}
// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "gfx/common/shader_description.hpp"

namespace Game
{
	struct shader_desc;

	class shader
	{
	public:
		bool create_from_file_vertex_pixel(const char* file);
		void destroy();

		inline uint16 get_hw() const
		{
			return _hw;
		}

		static void get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block);

		inline shader_desc& get_desc()
		{
			return _desc;
		}

	private:
		uint16 _hw = 0;

		shader_desc _desc = {
			.vertex_entry		 = "VSMain",
			.pixel_entry		 = "PSMain",
			.compute_entry		 = "CSMain",
			.flags				 = 0,
			.layout_data		 = {},
			.blobs				 = {},
			.attachments		 = {},
			.inputs				 = {},
			.depth_stencil_desc	 = {},
			.blend_logic_op		 = logic_op::and_,
			.topo				 = topology::triangle_list,
			.cull				 = cull_mode::back,
			.front				 = front_face::cw,
			.poly_mode			 = polygon_mode::fill,
			.layout				 = 0,
			.samples			 = 1,
			.depth_bias_constant = 0.0f,
			.depth_bias_clamp	 = 0.0f,
			.depth_bias_slope	 = 0.0f,
			.debug_name			 = "shader",
		};
	};

}

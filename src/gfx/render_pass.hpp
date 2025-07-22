// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace Game
{
	class vector2ui;

	class render_pass
	{
	public:
		struct indexed_draw
		{
			uint32 base_vertex		 = 0;
			uint32 index_count		 = 0;
			uint32 instance_count	 = 0;
			uint32 start_index		 = 0;
			uint32 start_instance	 = 0;
			uint32 shader_handle	 = 0;
			uint16 descriptor_handle = 0;
			uint16 descriptor_layout = 0;
		};

		struct per_frame_data
		{
			// vector<LinaGX::RenderPassColorAttachment> _color_attachments;
			// LinaGX::RenderPassDepthStencilAttachment  _depth_stencil_attachment = {};
		};

	public:
		void init(uint16* descriptor_sets, uint16 descriptor_layout);
		void uninit();

		void add_indexed_draw(indexed_draw draw);
		//	void render(LinaGX::CommandStream* cs, uint32 frame_index, const vector2ui& size);

		//	inline vector<LinaGX::RenderPassColorAttachment>& get_color_attachments(uint32 frame_index)
		//	{
		//		return _pfd[frame_index]._color_attachments;
		//	}
		//
		//	inline LinaGX::RenderPassDepthStencilAttachment& get_depth_stencil_attachment(uint32 frame_index)
		//	{
		//		return _pfd[frame_index]._depth_stencil_attachment;
		//	}

	private:
		vector<indexed_draw> _indexed_draws;
		uint16				 _descriptor_sets[FRAMES_IN_FLIGHT];
		uint16				 _descriptor_layout		= 0;
		per_frame_data		 _pfd[FRAMES_IN_FLIGHT] = {};
	};
}
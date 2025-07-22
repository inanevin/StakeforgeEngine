// Copyright (c) 2025 Inan Evin
#include "render_pass.hpp"
#include "math/vector2ui.hpp"

namespace Game
{
	void render_pass::init(uint16* descriptor_sets, uint16 descriptor_layout)
	{
		_descriptor_layout = descriptor_layout;
		_indexed_draws.reserve(100);

		//	for (uint32 i = 0; i < gfx_util::FRAMES_IN_FLIGHT; i++)
		//	{
		//		_descriptor_sets[i] = descriptor_sets[i];
		//	}
	}

	void render_pass::uninit()
	{
	}

	void render_pass::add_indexed_draw(indexed_draw draw)
	{
		_indexed_draws.push_back(draw);
	}

	// void render_pass::render(LinaGX::CommandStream* cs, uint32 frame_index, const vector2ui& size)
	//{
	//	per_frame_data& pfd = _pfd[frame_index];
	//	// Render pass begin
	//	{
	//		LinaGX::Viewport	 viewport = {.x = 0, .y = 0, .width = size.x, .height = size.y, .minDepth = 0.0f, .maxDepth = 1.0f};
	//		LinaGX::ScissorsRect sc		  = {.x = 0, .y = 0, .width = size.x, .height = size.y};
	//
	//		LinaGX::CMDBeginRenderPass* beginRenderPass = cs->AddCommand<LinaGX::CMDBeginRenderPass>();
	//		beginRenderPass->colorAttachmentCount		= 1;
	//		beginRenderPass->colorAttachments			= cs->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(pfd._color_attachments.data(), sizeof(LinaGX::RenderPassColorAttachment) * pfd._color_attachments.size());
	//		beginRenderPass->viewport					= viewport;
	//		beginRenderPass->scissors					= sc;
	//		beginRenderPass->depthStencilAttachment		= pfd._depth_stencil_attachment;
	//	}
	//
	//	LinaGX::CMDBindDescriptorSets* bind_ds = cs->AddCommand<LinaGX::CMDBindDescriptorSets>();
	//	bind_ds->setCount					   = 1;
	//	bind_ds->firstSet					   = 1;
	//	bind_ds->descriptorSetHandles		   = cs->EmplaceAuxMemory(_descriptor_sets[frame_index]);
	//	bind_ds->layoutSource				   = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
	//	bind_ds->customLayout				   = _descriptor_layout;
	//
	//	uint32 last_bound_shader = UINT32_MAX;
	//	uint16 last_material	 = UINT16_MAX;
	//
	//	auto bind_shader = [&](uint32 shader) {
	//		if (last_bound_shader == shader) return;
	//		LinaGX::CMDBindPipeline* bind = cs->AddCommand<LinaGX::CMDBindPipeline>();
	//		bind->shader				  = shader;
	//		last_bound_shader			  = shader;
	//	};
	//
	//	auto bind_material = [&](uint16 handle, uint16 layout) {
	//		if ((handle & (1u << 15)) == 0) return;
	//		const uint16 actual_handle = handle & ((1u << 15) - 1);
	//		if (last_material == actual_handle) return;
	//		LinaGX::CMDBindDescriptorSets* bind_ds = cs->AddCommand<LinaGX::CMDBindDescriptorSets>();
	//		bind_ds->setCount					   = 1;
	//		bind_ds->firstSet					   = 2;
	//		bind_ds->descriptorSetHandles		   = cs->EmplaceAuxMemory<uint16>(actual_handle);
	//		bind_ds->layoutSource				   = layout == 0 ? LinaGX::DescriptorSetsLayoutSource::LastBoundShader : LinaGX::DescriptorSetsLayoutSource::CustomLayout;
	//		bind_ds->customLayout				   = layout;
	//	};
	//
	//	for (const indexed_draw& draw : _indexed_draws)
	//	{
	//		bind_shader(draw.shader_handle);
	//		bind_material(draw.descriptor_handle, draw.descriptor_layout);
	//
	//		LinaGX::CMDDrawIndexedInstanced* cmd = cs->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
	//		cmd->baseVertexLocation				 = draw.base_vertex;
	//		cmd->indexCountPerInstance			 = draw.index_count;
	//		cmd->instanceCount					 = draw.instance_count;
	//		cmd->startIndexLocation				 = draw.start_index;
	//		cmd->startInstanceLocation			 = draw.start_instance;
	//	}
	//
	//	_indexed_draws.resize(0);
	//
	//	LinaGX::CMDEndRenderPass* end = cs->AddCommand<LinaGX::CMDEndRenderPass>();
	// }
}
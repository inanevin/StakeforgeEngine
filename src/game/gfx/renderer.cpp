// Copyright (c) 2025 Inan Evin
#include "renderer.hpp"
#include "game/game.hpp"
#include "platform/window.hpp"
#include "common/linagx_incl.hpp"
#include "io/log.hpp"
#include "data/vector_util.hpp"

#define VEKT_IMPL
#include "gui/vekt.hpp"

namespace Game
{
	void renderer::init(window& main_window)
	{
		const LinaGX::FormatSupportInfo sup_swp = LinaGX::instance()->GetFormatSupport(LinaGX::Format::B8G8R8A8_SRGB);
		const LinaGX::FormatSupportInfo sup_col = LinaGX::instance()->GetFormatSupport(LinaGX::Format::R8G8B8A8_SRGB);

		if (sup_swp.format == LinaGX::Format::UNDEFINED || !sup_col.colorAttachment || !sup_col.sampled) { throw std::exception("GPU format support failed!"); }

		_swapchain = LinaGX::instance()->CreateSwapchain({
			.format	  = LinaGX::Format::B8G8R8A8_SRGB,
			.width	  = main_window.get_size().x,
			.height	  = main_window.get_size().y,
			.window	  = main_window.get_window_handle(),
			.osHandle = main_window.get_platform_handle(),
			.vsyncStyle =
				LinaGX::VSyncStyle{
					.vulkanVsync = LinaGX::VKVsync::FIFO_RELAXED,
					.dx12Vsync	 = LinaGX::DXVsync::EveryVBlank,
				},
		});

		const LinaGX::DescriptorSetDesc global_set_description = {.bindings = {
																	  {
																		  .descriptorCount = 1,
																		  .type			   = LinaGX::DescriptorType::UBO,
																		  .stages		   = {LinaGX::ShaderStage::Vertex},
																	  },
																  }};

		const LinaGX::DescriptorSetDesc debug_gui_pass_description = {.bindings = {
																		  {
																			  .descriptorCount = 1,
																			  .type			   = LinaGX::DescriptorType::UBO,
																			  .stages		   = {LinaGX::ShaderStage::Vertex},
																		  },
																	  }};

		constexpr size_t gui_vertex_buffer_size = 1024 * 1024;
		constexpr size_t gui_index_buffer_size	= 1024 * 1024 * 2;

		for (uint32 i = 0; i < gfx_util::FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			pfd.stream_gfx		= LinaGX::instance()->CreateCommandStream({
					 .type				= LinaGX::CommandType::Graphics,
					 .commandCount		= 100,
					 .totalMemoryLimit	= 1024,
					 .auxMemorySize		= 1024,
					 .constantBlockSize = 64,
					 .debugName			= "Renderer Gfx Stream",
			 });

			pfd.stream_copy = LinaGX::instance()->CreateCommandStream({
				.type			   = LinaGX::CommandType::Transfer,
				.commandCount	   = 100,
				.totalMemoryLimit  = 1024,
				.auxMemorySize	   = 1024,
				.constantBlockSize = 64,
				.debugName		   = "Renderer Copy Stream",
			});

			pfd.descriptor_set_global	 = LinaGX::instance()->CreateDescriptorSet(global_set_description);
			pfd.descriptor_set_debug_gui = LinaGX::instance()->CreateDescriptorSet(debug_gui_pass_description);
			pfd.semaphore_copy			 = {LinaGX::instance()->CreateUserSemaphore()};

			pfd.buffer_gui_vertex.create(LinaGX::ResourceTypeHint::TH_VertexBuffer, static_cast<uint32>(gui_vertex_buffer_size), "Gui Vertex Buffer");
			pfd.buffer_gui_index.create(LinaGX::ResourceTypeHint::TH_IndexBuffer, static_cast<uint32>(gui_index_buffer_size), "Gui Index Buffer");
			pfd.buffer_global_data.create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(global_data), "Global Data Buffer");
			pfd.buffer_gui_ubo.create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(gui_data), "GUI Data Buffer");

			LinaGX::instance()->DescriptorUpdateBuffer({
				.setHandle = pfd.descriptor_set_global,
				.binding   = 0,
				.buffers   = {pfd.buffer_global_data.get_gpu()},
			});

			LinaGX::instance()->DescriptorUpdateBuffer({
				.setHandle = pfd.descriptor_set_debug_gui,
				.binding   = 0,
				.buffers   = {pfd.buffer_gui_ubo.get_gpu()},
			});
		}

		// pipeline layouts
		{
			_pipeline_layout_global = LinaGX::instance()->CreatePipelineLayout({
				.descriptorSetDescriptions = {global_set_description},
				.debugName				   = "Global Layout",
			});

			_pipeline_layout_debug_gui = LinaGX::instance()->CreatePipelineLayout({
				.descriptorSetDescriptions = {global_set_description, debug_gui_pass_description},
			});
		}

		// setup debug gui pass.
		{
			vector<uint16> sets;

			for (uint32 i = 0; i < gfx_util::FRAMES_IN_FLIGHT; i++)
				sets.push_back(_pfd[i].descriptor_set_debug_gui);

			_pass_debug_gui.init(sets.data(), _pipeline_layout_debug_gui);

			_pass_debug_gui.get_color_attachments(0).push_back(LinaGX::RenderPassColorAttachment{
				.clearColor	 = {32.0f / 255.0f, 17.0f / 255.0f, 39.0f / 255.0f, 1.0f},
				.texture	 = static_cast<uint32>(_swapchain),
				.isSwapchain = true,
			});

			_pass_debug_gui.get_color_attachments(1).push_back(LinaGX::RenderPassColorAttachment{
				.clearColor	 = {32.0f / 255.0f, 17.0f / 255.0f, 39.0f / 255.0f, 1.0f},
				.texture	 = static_cast<uint32>(_swapchain),
				.isSwapchain = true,
			});
		}

		_mesh_manager.init();

		vekt::config = {
			.on_log =
				[](vekt::log_verbosity verb, const char* log...) {
					if (verb == vekt::log_verbosity::error)
						GAME_ERR(log);
					else
						GAME_TRACE(log);
				},
			.atlas_width  = 1024,
			.atlas_height = 1024,
		};

		_sampler_gui_text.create({
			.minFilter	= LinaGX::Filter::Linear,
			.magFilter	= LinaGX::Filter::Linear,
			.mode		= LinaGX::SamplerAddressMode::ClampToBorder,
			.mipmapMode = LinaGX::MipmapMode::Linear,
		});

		vekt::font_manager::get().init();
		vekt::font_manager::get().set_atlas_created_callback([this](vekt::atlas* atlas) { atlas_created(atlas); });
		vekt::font_manager::get().set_atlas_updated_callback([this](vekt::atlas* atlas) { atlas_updated(atlas); });
		vekt::font_manager::get().set_atlas_destroyed_callback([this](vekt::atlas* atlas) { atlas_destroyed(atlas); });

		_font_debug = vekt::font_manager::get().load_font("assets/fonts/flexi-ibm.ttf", 16);

		// load fonts, which will create atlas textures.
		// start-end frame, upload atlases, wait for them.

		flush_copy_queue(_pfd[0]);

		_vekt_builder.init({
			.widget_count				 = 1000,
			.vertex_buffer_sz			 = gui_vertex_buffer_size,
			.index_buffer_sz			 = gui_index_buffer_size,
			.text_cache_vertex_buffer_sz = 1024 * 1024,
			.text_cache_index_buffer_sz	 = 1024 * 1024,
			.buffer_count				 = 50,
		});

		_vekt_builder.set_on_draw(std::bind(&renderer::on_gui_draw, this, std::placeholders::_1));

		_shader_gui_default.create_from_file_default("assets/shaders/gui/debug_default.glsl",
													 {
														 .colorAttachments = {{.format = LinaGX::Format::B8G8R8A8_SRGB,
																			   .blendAttachment =
																				   {
																					   .blendEnabled = true,
																				   }}},
														 .cullMode		   = LinaGX::CullMode::Back,
														 .frontFace		   = LinaGX::FrontFace::CW,
													 });

		_shader_gui_text.create_from_file_default("assets/shaders/gui/debug_text.glsl",
												  {
													  .colorAttachments = {{.format = LinaGX::Format::B8G8R8A8_SRGB,
																			.blendAttachment =
																				{
																					.blendEnabled = true,
																				}}},
													  .cullMode			= LinaGX::CullMode::Back,
													  .frontFace		= LinaGX::FrontFace::CW,
												  });

		_shader_gui_sdf.create_from_file_default("assets/shaders/gui/debug_sdf.glsl",
												 {
													 .colorAttachments = {{.format = LinaGX::Format::B8G8R8A8_SRGB,
																		   .blendAttachment =
																			   {
																				   .blendEnabled = true,
																			   }}},
													 .cullMode		   = LinaGX::CullMode::Back,
													 .frontFace		   = LinaGX::FrontFace::CW,
												 });

		vekt::id w = _vekt_builder.allocate();
		_vekt_builder.widget_set_pos(w, vekt::vec2(0.1f, 0.1f));
		_vekt_builder.widget_set_size(w, vekt::vec2(0.5f, 0.5f));
		_vekt_builder.widget_add_child(_vekt_builder.get_root(), w);
		vekt::widget_gfx& gfx = _vekt_builder.widget_get_gfx(w);
		gfx.flags			  = vekt::gfx_flags::gfx_is_text;
		gfx.color			  = vekt::vec4(0.9f, 0.9f, 0.9f, 1);
		vekt::text_props& txt = _vekt_builder.widget_get_text(w);
		txt.text			  = "world_load_level = level0.gltf";
		txt.font			  = _font_debug;
		_vekt_builder.widget_update_text(w);
	}

	void renderer::uninit()
	{
		_sampler_gui_text.destroy();

		_shader_gui_default.destroy();
		_shader_gui_text.destroy();
		_shader_gui_sdf.destroy();

		_vekt_builder.uninit();

		// will destroy atlases.
		vekt::font_manager::get().uninit();
		_atlas_refs.resize(0);

		_mesh_manager.uninit();
		_pass_debug_gui.uninit();

		LinaGX::instance()->DestroySwapchain(_swapchain);
		for (uint32 i = 0; i < gfx_util::FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			LinaGX::instance()->DestroyCommandStream(pfd.stream_gfx);
			LinaGX::instance()->DestroyCommandStream(pfd.stream_copy);
			LinaGX::instance()->DestroyDescriptorSet(pfd.descriptor_set_global);
			LinaGX::instance()->DestroyDescriptorSet(pfd.descriptor_set_debug_gui);
			LinaGX::instance()->DestroyUserSemaphore(pfd.semaphore_copy.semaphore());
			pfd.buffer_gui_vertex.destroy();
			pfd.buffer_gui_index.destroy();
			pfd.buffer_gui_ubo.destroy();
			pfd.buffer_global_data.destroy();
		}

		LinaGX::instance()->DestroyPipelineLayout(_pipeline_layout_global);
		LinaGX::instance()->DestroyPipelineLayout(_pipeline_layout_debug_gui);
	}

	void renderer::tick(float dt)
	{
	}

	void renderer::join()
	{
		LinaGX::instance()->Join();
	}

	void renderer::render(float alpha, const vector2ui& screen_size)
	{
		LinaGX::instance()->StartFrame();

		_frame_index		= LinaGX::instance()->GetCurrentFrameIndex();
		per_frame_data& pfd = _pfd[_frame_index];

		const global_data gb = {.delta_elapsed = vector4()};
		pfd.buffer_global_data.set_data(0, (uint8*)(&gb), sizeof(global_data));
		_resource_queue.add_buffer_request({.buffer = &pfd.buffer_global_data});

		const gui_data gui = {
			.projection = matrix4x4::ortho(0.0f, screen_size.x, 0.0f, screen_size.y, 0.0f, 1.0f),
		};
		pfd.buffer_gui_ubo.set_data(0, (uint8*)(&gui), sizeof(gui_data));
		_resource_queue.add_buffer_request({.buffer = &pfd.buffer_gui_ubo});

		pfd.vertex_counter_gui = 0;
		pfd.index_counter_gui  = 0;
		_vekt_builder.build(vekt::vec2(screen_size.x, screen_size.y));
		_vekt_builder.flush();

		_resource_queue.add_buffer_request({.buffer = &pfd.buffer_gui_vertex});
		_resource_queue.add_buffer_request({.buffer = &pfd.buffer_gui_index});

		flush_copy_queue(pfd);

		// Barrier to Color Attachment
		{
			LinaGX::CMDBarrier* barrier	 = pfd.stream_gfx->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
			barrier->dstStageFlags		 = LinaGX::PSF_ColorAttachment;
			barrier->textureBarrierCount = 1;
			barrier->textureBarriers	 = pfd.stream_gfx->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
			gfx_util::get_texture_barrier_present_2_color_att(static_cast<uint32>(_swapchain), barrier->textureBarriers[0]);
		}

		LinaGX::CMDBindDescriptorSets* bind_ds = pfd.stream_gfx->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bind_ds->setCount					   = 1;
		bind_ds->firstSet					   = 0;
		bind_ds->descriptorSetHandles		   = pfd.stream_gfx->EmplaceAuxMemory(pfd.descriptor_set_global);
		bind_ds->layoutSource				   = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bind_ds->customLayout				   = _pipeline_layout_global;

		pfd.buffer_gui_index.bind_index(pfd.stream_gfx, LinaGX::IndexType::Uint16);
		pfd.buffer_gui_vertex.bind_vertex(pfd.stream_gfx, sizeof(vekt::vertex));
		_pass_debug_gui.render(pfd.stream_gfx, _frame_index, screen_size);

		// Barrier to Present
		{
			LinaGX::CMDBarrier* barrier	 = pfd.stream_gfx->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment;
			barrier->dstStageFlags		 = LinaGX::PSF_BottomOfPipe;
			barrier->textureBarrierCount = 1;
			barrier->textureBarriers	 = pfd.stream_gfx->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
			gfx_util::get_texture_barrier_color_att_2_present(static_cast<uint32>(_swapchain), barrier->textureBarriers[0]);
		}

		LinaGX::instance()->CloseCommandStreams(&pfd.stream_gfx, 1);
		LinaGX::instance()->SubmitCommandStreams({.targetQueue = LinaGX::instance()->GetPrimaryQueue(LinaGX::CommandType::Graphics), .streams = &pfd.stream_gfx, .streamCount = 1});
		LinaGX::instance()->Present({.swapchains = &_swapchain, .swapchainCount = 1});

		LinaGX::instance()->EndFrame();
	}

	void renderer::on_window_resize(const vector2ui& size)
	{
		if (size.x == 0 || size.y == 0) return;

		join();
		LinaGX::instance()->RecreateSwapchain({
			.swapchain	  = _swapchain,
			.width		  = size.x,
			.height		  = size.y,
			.isFullscreen = false,
			.vsyncStyle =
				LinaGX::VSyncStyle{
					.vulkanVsync = LinaGX::VKVsync::FIFO_RELAXED,
					.dx12Vsync	 = LinaGX::DXVsync::EveryVBlank,
				},
			.scalingFactor = 1.0f,
		});
	}

	void renderer::atlas_created(vekt::atlas* atlas)
	{
		const uint32 texture = LinaGX::instance()->CreateTexture({
			.type	   = LinaGX::TextureType::Texture2D,
			.format	   = LinaGX::Format::R8_UNORM,
			.flags	   = LinaGX::TextureFlags::TF_Sampled | LinaGX::TextureFlags::TF_CopyDest,
			.width	   = atlas->get_width(),
			.height	   = atlas->get_height(),
			.debugName = "vekt_atlas",
		});

		_atlas_refs.push_back({
			.atlas = atlas,
			.hw	   = texture,
		});

		atlas_ref& ref = _atlas_refs.back();

		ref.mat.create(0,
					   {
						   .bindings =
							   {
								   {
									   .descriptorCount = 1,
									   .type			= LinaGX::DescriptorType::SeparateImage,
									   .stages			= {LinaGX::ShaderStage::Fragment},
								   },
								   {
									   .descriptorCount = 1,
									   .type			= LinaGX::DescriptorType::SeparateSampler,
									   .stages			= {LinaGX::ShaderStage::Fragment},
								   },
							   },
						   .allocationCount = 1,
					   });

		ref.mat.update_textures(0, {texture});
		ref.mat.update_samplers(1, {_sampler_gui_text.get_hw()});
	}

	void renderer::atlas_updated(vekt::atlas* atlas)
	{
		auto it = vector_util::find_if(_atlas_refs, [atlas](const atlas_ref& ref) { return ref.atlas == atlas; });
		if (it == _atlas_refs.end()) return;

		LinaGX::TextureBuffer buffer = {
			.pixels		   = atlas->get_data(),
			.width		   = atlas->get_width(),
			.height		   = atlas->get_height(),
			.bytesPerPixel = 1,
		};
		_resource_queue.add_texture_request(&buffer, 1, it->hw);

		/*
		_material.create(set_description);
		_material.set

		*/
	}

	void renderer::atlas_destroyed(vekt::atlas* atlas)
	{
		join();

		auto it = vector_util::find_if(_atlas_refs, [atlas](const atlas_ref& ref) { return ref.atlas == atlas; });
		if (it != _atlas_refs.end())
		{
			LinaGX::instance()->DestroyTexture(it->hw);
			it->hw = 0;
			it->mat.destroy();
			_atlas_refs.erase(it);
		}
	}

	void renderer::on_gui_draw(const vekt::draw_buffer& buffer)
	{
		per_frame_data& pfd = _pfd[_frame_index];
		pfd.buffer_gui_vertex.set_data(static_cast<size_t>(pfd.vertex_counter_gui) * sizeof(vekt::vertex), reinterpret_cast<uint8*>(buffer.vertex_start), sizeof(vekt::vertex) * buffer.vertex_count);
		pfd.buffer_gui_index.set_data(static_cast<size_t>(pfd.index_counter_gui) * sizeof(vekt::index), reinterpret_cast<uint8*>(buffer.index_start), sizeof(vekt::index) * buffer.index_count);

		uint32 shader	  = _shader_gui_default.get_hw();
		uint16 descriptor = 0;

		if (buffer.used_font != nullptr)
		{
			shader	= buffer.used_font->type == vekt::font_type::normal ? _shader_gui_text.get_hw() : _shader_gui_sdf.get_hw();
			auto it = vector_util::find_if(_atlas_refs, [&](const atlas_ref& ref) -> bool { return ref.atlas == buffer.used_font->_atlas; });
			if (it != _atlas_refs.end()) descriptor = it->mat.get_hw() | (1u << 15);
		}

		_pass_debug_gui.add_indexed_draw({
			.base_vertex	   = pfd.vertex_counter_gui,
			.index_count	   = buffer.index_count,
			.instance_count	   = 1,
			.start_index	   = pfd.index_counter_gui,
			.start_instance	   = 0,
			.shader_handle	   = shader,
			.descriptor_handle = descriptor,
		});

		pfd.vertex_counter_gui += buffer.vertex_count;
		pfd.index_counter_gui += buffer.index_count;
	}

	void renderer::flush_copy_queue(per_frame_data& pfd)
	{
		if (_resource_queue.flush_all(pfd.stream_copy))
		{
			pfd.semaphore_copy.increment();

			LinaGX::instance()->CloseCommandStreams(&pfd.stream_copy, 1);
			LinaGX::instance()->SubmitCommandStreams({
				.targetQueue	  = LinaGX::instance()->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &pfd.stream_copy,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = pfd.semaphore_copy.semaphore_ptr(),
				.signalValues	  = pfd.semaphore_copy.value_ptr(),
			});

			LinaGX::instance()->WaitForUserSemaphore(pfd.semaphore_copy.semaphore(), pfd.semaphore_copy.value());
		}
	}
}
// Copyright (c) 2025 Inan Evin
#include "renderer.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/gfx_util.hpp"
#include "platform/window.hpp"
#include "math/vector4.hpp"

namespace Game
{
	void renderer::init(const window& main_window)
	{
		gfx_backend::s_instance = new gfx_backend();
		gfx_backend::get()->init();

		_frame_allocator.init(1024 * 1024, 4);

		_swapchain_main = gfx_backend::get()->create_swapchain({
			.window	   = main_window.get_window_handle(),
			.os_handle = main_window.get_platform_handle(),
			.scaling   = 1.0f,
			.format	   = format::b8g8r8a8_unorm,
			.pos	   = vector2ui::zero,
			.size	   = main_window.get_size(),
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});

		_bind_layout_gui_default = gfx_backend::get()->create_bind_layout({
			.bindings =
				{
					{
						.entry_table =
							{
								{
									.type	 = descriptor_type::ubo,
									.count	 = 1,
									.set	 = 0,
									.binding = 0,
								},
							},
						.visibility = shader_stage::all,
					},
					{
						.entry_table =
							{
								{
									.type					= descriptor_type::sampler,
									.count					= 1,
									.set					= 0,
									.binding				= 0,
									.immutable_sampler_desc = gfx_util::get_sampler_desc_gui_default(),
								},
							},
						.visibility = shader_stage::all,
					},
				},

		});

		_shader_gui_default.create_from_file_vertex_pixel("assets/engine/gui/gui_default.hlsl",
														  {
															  .vertex_entry = "VSMain",
															  .pixel_entry	= "PSMain",
															  .flags		= 0,
															  .attachments =
																  {
																	  {
																		  .format			= format::b8g8r8a8_unorm,
																		  .blend_attachment = gfx_util::get_blend_attachment_alpha_blending(),
																	  },
																  },
															  .inputs =
																  {
																	  {
																		  .name		= "POSITION",
																		  .location = 0,
																		  .index	= 0,
																		  .offset	= 0,
																		  .size		= sizeof(vector2),
																		  .format	= format::r32g32_sfloat,
																	  },
																	  {
																		  .name		= "TEXCOORD",
																		  .location = 0,
																		  .index	= 0,
																		  .offset	= sizeof(vector2),
																		  .size		= sizeof(vector2),
																		  .format	= format::r32g32_sfloat,
																	  },
																	  {
																		  .name		= "COLOR",
																		  .location = 0,
																		  .index	= 0,
																		  .offset	= sizeof(vector2) * 2,
																		  .size		= sizeof(vector4),
																		  .format	= format::r32g32b32a32_sfloat,
																	  },
																  },
															  .depth_stencil_desc =
																  {
																	  .flags = 0,
																  },
															  .topo		  = topology::triangle_list,
															  .cull		  = cull_mode::back,
															  .front	  = front_face::cw,
															  .poly_mode  = polygon_mode::fill,
															  .samples	  = 1,
															  .debug_name = "gui_default",
														  });

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd			  = _pfd[i];
			pfd.frame_semaphore.semaphore = gfx_backend::get()->create_semaphore();
		}
	}

	void renderer::uninit()
	{
		_shader_gui_default.destroy();

		gfx_backend::get()->destroy_bind_layout(_bind_layout_gui_default);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			gfx_backend::get()->destroy_semaphore(pfd.frame_semaphore.semaphore);
		}

		gfx_backend::get()->destroy_swapchain(_swapchain_main);

		_frame_allocator.uninit();

		gfx_backend::get()->uninit();
		delete gfx_backend::s_instance;
		gfx_backend::s_instance = nullptr;
	}

	void renderer::wait_backend()
	{
		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->wait_semaphore(pfd.frame_semaphore.semaphore, pfd.frame_semaphore.value);
		}
	}

	void renderer::populate_render_data(uint8 index)
	{
		render_data& write_data = _render_data[index];
		reset_render_data(index);
	}

	void renderer::render(uint8 index, const vector2ui& size)
	{
		render_data& read_data = _render_data[index];
		_frame_allocator.reset();

		gfx_backend*	backend = gfx_backend::get();
		per_frame_data& pfd		= _pfd[_frame_index];

		backend->wait_semaphore(pfd.frame_semaphore.semaphore, pfd.frame_semaphore.value);

		// begin gui pass on swapchain

		//
		// flush vekt, get draw comamnds, bind shader and call draw commands on backend.
		// lets think of root signature.

		// end gui pass on swapchain

		backend->present(&_swapchain_main, 1);

		pfd.frame_semaphore.value++;
		backend->queue_signal(backend->get_queue_gfx(), &pfd.frame_semaphore.semaphore, 1, &pfd.frame_semaphore.value);

		_frame_index = (_frame_index + 1) % FRAMES_IN_FLIGHT;
	}

	bool renderer::on_window_event(const window_event& ev)
	{
		return false;
	}

	void renderer::on_window_resize(const vector2ui& size)
	{
		gfx_backend* backend = gfx_backend::get();
		backend->recreate_swapchain({
			.size	   = size,
			.swapchain = _swapchain_main,
			.format	   = format::b8g8r8a8_unorm,
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});
	}

	void renderer::reset_render_data(uint8 index)
	{
	}

}
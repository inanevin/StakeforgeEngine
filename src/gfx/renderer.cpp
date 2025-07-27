
// Copyright (c) 2025 Inan Evin
#include "renderer.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/common/commands.hpp"
#include "gfx/gfx_util.hpp"
#include "platform/window.hpp"
#include "math/vector4.hpp"
#include "memory/memory_tracer.hpp"
#include "io/log.hpp"
#include "common/system_info.hpp"

namespace Game
{
#define RT_FORMAT format::r8g8b8a8_srgb

	void renderer::init(const window& main_window)
	{
		gfx_backend::s_instance = new gfx_backend();
		gfx_backend* backend	= gfx_backend::get();
		backend->init();

		_gfx_data.swapchain_main = backend->create_swapchain({
			.window	   = main_window.get_window_handle(),
			.os_handle = main_window.get_platform_handle(),
			.scaling   = 1.0f,
			.format	   = format::r8g8b8a8_srgb,
			.pos	   = vector2ui16::zero,
			.size	   = main_window.get_size(),
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});

		_gfx_data.bind_layout_global = gfx_util::create_bind_layout_global();

		_gfx_data.dummy_sampler = backend->create_sampler({});
		_gfx_data.dummy_texture = backend->create_texture({
			.texture_format = format::r8_unorm,
			.size			= vector2ui16(1, 1),
			.flags			= texture_flags::tf_is_2d | texture_flags::tf_sampled,
		});
		_gfx_data.dummy_ubo		= backend->create_resource({.size = 4, .flags = resource_flags::rf_constant_buffer | resource_flags::rf_gpu_only});
		_gfx_data.dummy_ssbo	= backend->create_resource({.size = 4, .flags = resource_flags::rf_storage_buffer | resource_flags::rf_gpu_only});

		_shaders.swapchain.get_desc().attachments = {{.format = RT_FORMAT, .blend_attachment = gfx_util::get_blend_attachment_alpha_blending()}};
		_shaders.swapchain.get_desc().inputs	  = gfx_util::get_input_layout(input_layout_type::gui_default);
		_shaders.swapchain.get_desc().cull		  = cull_mode::back;
		_shaders.swapchain.get_desc().front		  = front_face::cw;
		_shaders.swapchain.get_desc().layout	  = _gfx_data.bind_layout_global;
		_shaders.swapchain.get_desc().set_name("swapchain");
		_shaders.swapchain.create_from_file_vertex_pixel("assets/engine/shaders/swapchain/swapchain.hlsl");

		_debug_controller.init(&_texture_queue, _gfx_data.bind_layout_global, main_window.get_size());

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd		= _pfd[i];
			pfd.sem_frame.semaphore = backend->create_semaphore();
			pfd.sem_copy.semaphore	= backend->create_semaphore();
			pfd.cmd_gfx				= backend->create_command_buffer({
							.type		= command_type::graphics,
							.debug_name = "renderer_gfx",
			});

			pfd.cmd_copy = backend->create_command_buffer({
				.type		= command_type::transfer,
				.debug_name = "renderer_copy",
			});

			pfd.buf_engine_global.create_hw({.size = sizeof(buf_engine_global), .flags = resource_flags::rf_constant_buffer | resource_flags::rf_cpu_visible, .debug_name = "engine_globals"});
			pfd.bind_group_global = gfx_util::create_bind_group_global();

			backend->bind_group_update_descriptor(pfd.bind_group_global, 0, pfd.buf_engine_global.get_hw_gpu());
			gfx_util::update_dummy_bind_group(pfd.bind_group_global, _gfx_data.dummy_texture, _gfx_data.dummy_sampler, _gfx_data.dummy_ssbo, _gfx_data.dummy_ubo);

			pfd.bind_group_swapchain = backend->create_empty_bind_group();
			backend->bind_group_add_pointer(pfd.bind_group_swapchain, rpi_table_material, 3, false);
			backend->bind_group_update_pointer(pfd.bind_group_swapchain, 0, {{.resource = _debug_controller.get_final_rt(i), .view = 0, .pointer_index = upi_material_texture0, .type = binding_type::texture}});
			_frame_allocator[i].init(1024 * 1024, 4);
		}

		_buffer_queue.init();
		_texture_queue.init();
		_reuse_barriers.reserve(256);
	}

	void renderer::uninit()
	{
		_shaders.swapchain.destroy();

		_debug_controller.uninit();
		_texture_queue.uninit();
		_buffer_queue.uninit();

		gfx_backend* backend = gfx_backend::get();
		backend->destroy_bind_layout(_gfx_data.bind_layout_global);

		backend->destroy_resource(_gfx_data.dummy_ubo);
		backend->destroy_resource(_gfx_data.dummy_ssbo);
		backend->destroy_sampler(_gfx_data.dummy_sampler);
		backend->destroy_texture(_gfx_data.dummy_texture);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_bind_group(pfd.bind_group_swapchain);
			backend->destroy_semaphore(pfd.sem_frame.semaphore);
			backend->destroy_semaphore(pfd.sem_copy.semaphore);
			backend->destroy_command_buffer(pfd.cmd_gfx);
			backend->destroy_command_buffer(pfd.cmd_copy);
			backend->destroy_bind_group(pfd.bind_group_global);
			pfd.buf_engine_global.destroy();
			_frame_allocator[i].uninit();
		}

		backend->destroy_swapchain(_gfx_data.swapchain_main);

		backend->uninit();
		delete gfx_backend::s_instance;
		gfx_backend::s_instance = nullptr;
	}

	void renderer::wait_backend()
	{
		GAME_TRACE("call: renderer::wait_backend()");

		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->wait_semaphore(pfd.sem_frame.semaphore, pfd.sem_frame.value);
		}

		_gfx_data.frame_index = 0;
	}

	void renderer::populate_render_data(uint8 index)
	{
		render_data& write_data = _render_data[index];
		reset_render_data(index);
	}

	/*

void renderer::render(uint8 index, const vector2ui16& size)
	{
		gfx_backend*	  backend		= gfx_backend::get();
		render_data&	  read_data		= _render_data[index];
		const resource_id queue_gfx		= backend->get_queue_gfx();
		const resource_id frame_index	= _gfx_data.frame_index;
		const resource_id swp			= _gfx_data.swapchain_main;
		const resource_id layout_global = _gfx_data.bind_layout_global;
		_gfx_data.frame_index			= (_gfx_data.frame_index + 1) % FRAMES_IN_FLIGHT;

		bump_allocator& alloc = _frame_allocator[frame_index];
		alloc.reset();

		per_frame_data&	  pfd				 = _pfd[frame_index];
		const resource_id frame_sem			 = pfd.sem_frame.semaphore;
		const resource_id copy_sem			 = pfd.sem_copy.semaphore;
		const uint64	  frame_sem_val		 = pfd.sem_frame.value;
		const uint64	  frame_sem_val_incr = pfd.sem_frame.value + 1;
		const uint64	  frame_sem_copy_val = pfd.sem_copy.value;
		const resource_id cmd_list			 = pfd.cmd_gfx;
		const resource_id bg_global			 = pfd.bind_group_global;
		const resource_id bg_swap			 = pfd.bind_group_swapchain;
		const resource_id shader_swp		 = _shaders.swapchain.get_hw();
		pfd.sem_frame.value++;


		backend->wait_semaphore(frame_sem, frame_sem_val);

		const buf_engine_global globals = {};
		pfd.buf_engine_global.buffer_data(0, (void*)&globals, sizeof(buf_engine_global));

		_debug_controller.upload(_buffer_queue, frame_index);
		send_uploads();

		const uint64 frame_sem_copy_val_incr = pfd.sem_copy.value;



		// Begin frame cmd list
		backend->reset_command_buffer(cmd_list);
		backend->cmd_bind_layout(cmd_list, {.layout = layout_global});
		backend->cmd_bind_group(cmd_list, {.group = bg_global});

		_reuse_barriers.push_back({
			.resource	= swp,
			.flags		= barrier_flags::baf_is_swapchain,
			.from_state = resource_state::present,
			.to_state	= resource_state::render_target,
		});
		_debug_controller.collect_barriers(_reuse_barriers);
		send_barriers(cmd_list);

		// Render debug data
		_debug_controller.render(cmd_list, frame_index, alloc);

		// swapchain pass
		{
			render_pass_color_attachment* attachment = alloc.allocate<render_pass_color_attachment>(1);
			attachment->clear_color					 = vector4(0.8f, 0.7f, 0.7f, 1.0f);
			attachment->load_op						 = load_op::clear;
			attachment->store_op					 = store_op::store;
			attachment->texture						 = swp;

			backend->cmd_begin_render_pass_swapchain(cmd_list, {.color_attachments = attachment, .color_attachment_count = 1});
			backend->cmd_set_scissors(cmd_list, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_set_viewport(cmd_list, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_bind_group(cmd_list, {.group = bg_swap});
			backend->cmd_bind_pipeline(cmd_list, {.pipeline = shader_swp});
			backend->cmd_draw_instanced(cmd_list, {.vertex_count_per_instance = 6, .instance_count = 1});
			backend->cmd_end_render_pass(cmd_list, {});
		}

		// Rt -> Present Barrier
		{
			_reuse_barriers.push_back({
				.resource	= swp,
				.flags		= barrier_flags::baf_is_swapchain,
				.from_state = resource_state::render_target,
				.to_state	= resource_state::present,
			});
			send_barriers(cmd_list);
		}

		backend->close_command_buffer(cmd_list);

		if (frame_sem_copy_val != frame_sem_copy_val_incr)
			backend->queue_wait(queue_gfx, &copy_sem, 1, &frame_sem_copy_val_incr);

		backend->submit_commands(queue_gfx, &cmd_list, 1);
		backend->present(&swp, 1);
		backend->queue_signal(queue_gfx, &frame_sem, 1, &frame_sem_val_incr);
	}

	*/
	void renderer::render(uint8 index, const vector2ui16& size)
	{
		gfx_backend*	  backend	= gfx_backend::get();
		render_data&	  read_data = _render_data[index];
		const resource_id queue_gfx = backend->get_queue_gfx();

		/* access frame data */
		const uint8		  frame_index	= _gfx_data.frame_index;
		const resource_id layout_global = _gfx_data.bind_layout_global;
		const resource_id swapchain		= _gfx_data.swapchain_main;
		_gfx_data.frame_index			= (_gfx_data.frame_index + 1) % FRAMES_IN_FLIGHT;

		per_frame_data& pfd	  = _pfd[frame_index];
		bump_allocator& alloc = _frame_allocator[frame_index];
		alloc.reset();

		/*
			Wait for frame's fence, then send any uploads needed.
		*/
		backend->wait_semaphore(pfd.sem_frame.semaphore, pfd.sem_frame.value);

		const buf_engine_global globals = {};
		pfd.buf_engine_global.buffer_data(0, (void*)&globals, sizeof(buf_engine_global));

		/* access pfd */
		const resource_id cmd_list			  = pfd.cmd_gfx;
		const resource_id bg_global			  = pfd.bind_group_global;
		const resource_id bg_swapchain		  = pfd.bind_group_swapchain;
		const resource_id shader_swp		  = _shaders.swapchain.get_hw();
		const resource_id sem_frame			  = pfd.sem_frame.semaphore;
		const resource_id sem_copy			  = pfd.sem_copy.semaphore;
		const uint64	  previous_copy_value = pfd.sem_copy.value;
		const uint64	  current_frame_value = pfd.sem_frame.value;
		const uint64	  next_frame_value	  = ++pfd.sem_frame.value;
		_debug_controller.upload(_buffer_queue, frame_index);
		send_uploads(frame_index);
		const uint64 next_copy_value = pfd.sem_copy.value;

		/*
			Start frame command list.
			Transition swapchain, then render console & retransition.
		*/

		// Begin frame cmd list
		backend->reset_command_buffer(cmd_list);
		backend->cmd_bind_layout(cmd_list, {.layout = layout_global});
		backend->cmd_bind_group(cmd_list, {.group = bg_global});

		_reuse_barriers.push_back({
			.resource	= swapchain,
			.flags		= barrier_flags::baf_is_swapchain,
			.from_state = resource_state::present,
			.to_state	= resource_state::render_target,
		});
		_debug_controller.collect_barriers(_reuse_barriers);
		send_barriers(cmd_list);

		// Render debug data
		_debug_controller.render(cmd_list, frame_index, alloc);

		// swapchain pass
		{
			render_pass_color_attachment* attachment = alloc.allocate<render_pass_color_attachment>(1);
			attachment->clear_color					 = vector4(0.8f, 0.7f, 0.7f, 1.0f);
			attachment->load_op						 = load_op::clear;
			attachment->store_op					 = store_op::store;
			attachment->texture						 = swapchain;

			backend->cmd_begin_render_pass_swapchain(cmd_list, {.color_attachments = attachment, .color_attachment_count = 1});
			backend->cmd_set_scissors(cmd_list, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_set_viewport(cmd_list, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_bind_group(cmd_list, {.group = bg_swapchain});
			backend->cmd_bind_pipeline(cmd_list, {.pipeline = shader_swp});
			backend->cmd_draw_instanced(cmd_list, {.vertex_count_per_instance = 6, .instance_count = 1});
			backend->cmd_end_render_pass(cmd_list, {});
		}

		// Rt -> Present Barrier
		{
			_reuse_barriers.push_back({
				.resource	= swapchain,
				.flags		= barrier_flags::baf_is_swapchain,
				.from_state = resource_state::render_target,
				.to_state	= resource_state::present,
			});
			send_barriers(cmd_list);
		}

		/*
			End the frame command list.
			Insert queue wait if there were any uploads.
			Submit & present, then insert queue signal for this frame's fence.
		*/
		backend->close_command_buffer(cmd_list);

		if (previous_copy_value != next_copy_value)
			backend->queue_wait(queue_gfx, &sem_copy, 1, &next_copy_value);

		backend->submit_commands(queue_gfx, &cmd_list, 1);
		backend->present(&swapchain, 1);

		backend->queue_signal(queue_gfx, &sem_frame, 1, &next_frame_value);
	}

	bool renderer::on_window_event(const window_event& ev)
	{
		return _debug_controller.on_window_event(ev);
	}

	void renderer::on_window_resize(const vector2ui16& size)
	{
		VERIFY_THREAD_MAIN();

		gfx_backend* backend = gfx_backend::get();
		backend->recreate_swapchain({
			.size	   = size,
			.swapchain = _gfx_data.swapchain_main,
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});

		_debug_controller.on_window_resize(size);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			backend->bind_group_update_pointer(_pfd[i].bind_group_swapchain, 0, {{.resource = _debug_controller.get_final_rt(i), .view = 0, .pointer_index = upi_material_texture0, .type = binding_type::texture}});
		}
	}

	void renderer::reset_render_data(uint8 index)
	{
	}

	void renderer::send_uploads(uint8 frame_index)
	{
		per_frame_data&	  pfd	  = _pfd[frame_index];
		gfx_backend*	  backend = gfx_backend::get();
		const resource_id queue	  = backend->get_queue_transfer();
		if (!_buffer_queue.empty() || !_texture_queue.empty())
		{
			pfd.sem_copy.value++;
			backend->reset_command_buffer(pfd.cmd_copy);
			_buffer_queue.flush_all(pfd.cmd_copy);
			_texture_queue.flush_all(pfd.cmd_copy);
			backend->close_command_buffer(pfd.cmd_copy);
			backend->submit_commands(queue, &pfd.cmd_copy, 1);
			backend->queue_signal(queue, &pfd.sem_copy.semaphore, 1, &pfd.sem_copy.value);
		}
	}

	void renderer::send_barriers(resource_id cmd_list)
	{
		gfx_backend* backend = gfx_backend::get();
		backend->cmd_barrier(cmd_list,
							 {
								 .barriers		= _reuse_barriers.data(),
								 .barrier_count = static_cast<uint16>(_reuse_barriers.size()),
							 });

		_reuse_barriers.resize(0);
	}
}
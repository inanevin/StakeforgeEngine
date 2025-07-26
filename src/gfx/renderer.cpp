
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

			_frame_allocator[i].init(1024 * 1024, 4);
		}

		_buffer_queue.init();
		_texture_queue.init();
		_debug_controller.init(&_texture_queue, _gfx_data.bind_layout_global, main_window.get_size());
		_reuse_barriers.reserve(256);
	}

	void renderer::uninit()
	{
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

	void renderer::render(uint8 index, const vector2ui16& size)
	{
		gfx_backend*	  backend	= gfx_backend::get();
		const resource_id queue_gfx = backend->get_queue_gfx();
		render_data&	  read_data = _render_data[index];
		per_frame_data&	  pfd		= _pfd[_gfx_data.frame_index];
		bump_allocator&	  alloc		= _frame_allocator[_gfx_data.frame_index];
		alloc.reset();

		/*
			Wait for frame's fence, then send any uploads needed.
		*/
		backend->wait_semaphore(pfd.sem_frame.semaphore, pfd.sem_frame.value);

		const buf_engine_global globals = {};
		pfd.buf_engine_global.buffer_data(0, (void*)&globals, sizeof(buf_engine_global));

		_debug_controller.upload(_buffer_queue, _gfx_data.frame_index);
		const uint64 previous_copy_value = pfd.sem_copy.value;
		send_uploads();

		/*
			Start frame command list.
			Transition swapchain, then render console & retransition.
		*/

		// Begin frame cmd list
		backend->reset_command_buffer(pfd.cmd_gfx);
		backend->cmd_bind_layout(pfd.cmd_gfx, {.layout = _gfx_data.bind_layout_global});
		backend->cmd_bind_group(pfd.cmd_gfx, {.group = pfd.bind_group_global});

		_reuse_barriers.push_back({
			.resource	= _gfx_data.swapchain_main,
			.flags		= barrier_flags::baf_is_swapchain,
			.from_state = resource_state::present,
			.to_state	= resource_state::render_target,
		});
		_debug_controller.collect_barriers(_reuse_barriers);
		send_barriers(pfd.cmd_gfx);

		// Render debug data
		_debug_controller.render(pfd.cmd_gfx, _gfx_data.frame_index, alloc);

		// swapchain pass
		{
			render_pass_color_attachment* attachment = alloc.allocate<render_pass_color_attachment>(1);
			attachment->clear_color					 = vector4(0.8f, 0.7f, 0.7f, 1.0f);
			attachment->load_op						 = load_op::clear;
			attachment->store_op					 = store_op::store;
			attachment->texture						 = _gfx_data.swapchain_main;

			backend->cmd_begin_render_pass_swapchain(pfd.cmd_gfx, {.color_attachments = attachment, .color_attachment_count = 1});
			backend->cmd_set_scissors(pfd.cmd_gfx, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			backend->cmd_set_viewport(pfd.cmd_gfx, {.width = static_cast<uint16>(size.x), .height = static_cast<uint16>(size.y)});
			// backend->cmd_bind_pipeline(cmd_buffer, {.pipeline = _shaders.swapchain_gui.get_hw()});
			// backend->cmd_draw_instanced(cmd_buffer, {.vertex_count_per_instance = 6, .instance_count = 1});
			backend->cmd_end_render_pass(pfd.cmd_gfx, {});
		}

		// Rt -> Present Barrier
		{
			_reuse_barriers.push_back({
				.resource	= _gfx_data.swapchain_main,
				.flags		= barrier_flags::baf_is_swapchain,
				.from_state = resource_state::render_target,
				.to_state	= resource_state::present,
			});
			send_barriers(pfd.cmd_gfx);
		}

		/*
			End the frame command list.
			Insert queue wait if there were any uploads.
			Submit & present, then insert queue signal for this frame's fence.
		*/
		backend->close_command_buffer(pfd.cmd_gfx);

		if (previous_copy_value != pfd.sem_copy.value)
			backend->queue_wait(queue_gfx, &pfd.sem_copy.semaphore, 1, &pfd.sem_copy.value);

		backend->submit_commands(queue_gfx, &pfd.cmd_gfx, 1);
		backend->present(&_gfx_data.swapchain_main, 1);

		pfd.sem_frame.value++;
		backend->queue_signal(queue_gfx, &pfd.sem_frame.semaphore, 1, &pfd.sem_frame.value);

		_gfx_data.frame_index = (_gfx_data.frame_index + 1) % FRAMES_IN_FLIGHT;
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
	}

	void renderer::reset_render_data(uint8 index)
	{
	}

	void renderer::send_uploads()
	{
		per_frame_data&	  pfd	  = _pfd[_gfx_data.frame_index];
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
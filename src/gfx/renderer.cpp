
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

		_swapchain_main = backend->create_swapchain({
			.window	   = main_window.get_window_handle(),
			.os_handle = main_window.get_platform_handle(),
			.scaling   = 1.0f,
			.format	   = format::r8g8b8a8_srgb,
			.pos	   = vector2ui16::zero,
			.size	   = main_window.get_size(),
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});

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

			_frame_allocator[i].init(1024 * 1024, 4);
		}

		_buffer_queue.init();
		_texture_queue.init();
		_debug_controller.init(&_texture_queue, main_window.get_size());
		_reuse_barriers.reserve(256);
	}

	void renderer::uninit()
	{
		_debug_controller.uninit();
		_texture_queue.uninit();
		_buffer_queue.uninit();

		gfx_backend* backend = gfx_backend::get();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			per_frame_data& pfd = _pfd[i];
			backend->destroy_semaphore(pfd.sem_frame.semaphore);
			backend->destroy_semaphore(pfd.sem_copy.semaphore);

			backend->destroy_command_buffer(pfd.cmd_gfx);
			backend->destroy_command_buffer(pfd.cmd_copy);
			_frame_allocator[i].uninit();
		}

		backend->destroy_swapchain(_swapchain_main);

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

		_frame_index = 0;
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
		per_frame_data&	  pfd		= _pfd[_frame_index];
		bump_allocator&	  alloc		= _frame_allocator[_frame_index];
		alloc.reset();

		/*
			Wait for frame's fence, then send any uploads needed.
		*/

		backend->wait_semaphore(pfd.sem_frame.semaphore, pfd.sem_frame.value);
		_debug_controller.upload(_buffer_queue, _frame_index, size);
		const uint64 previous_copy_value = pfd.sem_copy.value;
		send_uploads();

		/*
			Start frame command list.
			Transition swapchain, then render console & retransition.
		*/

		// Begin frame cmd list
		backend->reset_command_buffer(pfd.cmd_gfx);

		_reuse_barriers.push_back({
			.resource	= _swapchain_main,
			.flags		= barrier_flags::baf_is_swapchain,
			.from_state = resource_state::present,
			.to_state	= resource_state::render_target,
		});
		_debug_controller.collect_barriers(_reuse_barriers);
		send_barriers(pfd.cmd_gfx);

		// Console stuff
		_debug_controller.render(pfd.cmd_gfx, _swapchain_main, _frame_index, size, alloc);

		// Rt -> Present Barrier
		{
			_reuse_barriers.push_back({
				.resource	= _swapchain_main,
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
		backend->present(&_swapchain_main, 1);

		pfd.sem_frame.value++;
		backend->queue_signal(queue_gfx, &pfd.sem_frame.semaphore, 1, &pfd.sem_frame.value);

		_frame_index = (_frame_index + 1) % FRAMES_IN_FLIGHT;
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
			.swapchain = _swapchain_main,
			.flags	   = swapchain_flags::sf_allow_tearing | swapchain_flags::sf_vsync_every_v_blank,
		});

		_debug_controller.on_window_resize(size);
	}

	void renderer::reset_render_data(uint8 index)
	{
	}

	void renderer::send_uploads()
	{
		per_frame_data&	  pfd	  = _pfd[_frame_index];
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
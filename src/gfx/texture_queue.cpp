// Copyright (c) 2025 Inan Evin

#include "texture_queue.hpp"
#include "data/vector_util.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/commands.hpp"

namespace Game
{

	void texture_queue::init()
	{
		_callbacks.reserve(256);
		_requests.reserve(256);
	}

	void texture_queue::uninit()
	{
	}

	void texture_queue::add_request(const texture_request& req)
	{
		auto it = vector_util::find_if(_requests, [&](const texture_request& existing) -> bool { return req.texture == existing.texture; });
		if (it != _requests.end())
		{
			it->buffers		 = req.buffers;
			it->buffer_count = req.buffer_count;
			return;
		}

		_requests.push_back(req);
	}

	void texture_queue::flush_all(resource_id cmd_list)
	{
		gfx_backend* backend = gfx_backend::get();

		for (const texture_request& buf : _requests)
		{
			backend->cmd_copy_buffer_to_texture(cmd_list,
												{
													.textures			 = buf.buffers,
													.destination_texture = buf.texture,
													.intermediate_buffer = buf.intermediate,
													.mip_levels			 = buf.buffer_count,
													.destination_slice	 = 0,

												});
		}

		_requests.resize(0);

		for (flush_callback cb : _callbacks)
			cb();

		_callbacks.resize(0);
	}

	bool texture_queue::empty() const
	{
		return _requests.empty();
	}

	void texture_queue::subscribe_flush_callback(flush_callback&& cb)
	{
		_callbacks.push_back(std::forward<flush_callback>(cb));
	}

} // namespace Lina

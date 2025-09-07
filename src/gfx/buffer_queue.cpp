// Copyright (c) 2025 Inan Evin

#include "buffer_queue.hpp"
#include "gfx/buffer.hpp"

namespace SFG
{

	void buffer_queue::init()
	{
		_requests.reserve(256);
	}

	void buffer_queue::uninit()
	{
	}

	void buffer_queue::add_request(const buffer_request& req)
	{
		_requests.push_back(req);
	}

	void buffer_queue::flush_all(gfx_id cmd_list)
	{
		for (const buffer_request& buf : _requests)
			buf.buffer->copy(cmd_list);

		_requests.resize(0);
	}

	bool buffer_queue::empty() const
	{
		for (const buffer_request& buf : _requests)
		{
			if (buf.buffer->is_dirty())
				return false;
		}

		return true;
	}
} // namespace Lina

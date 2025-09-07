// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/vector.hpp"
#include "gfx/common/gfx_constants.hpp"

namespace SFG
{
	class buffer;

	class buffer_queue
	{
	public:
		struct buffer_request
		{
			buffer* buffer = nullptr;
		};

		void init();
		void uninit();
		void add_request(const buffer_request& req);
		void flush_all(gfx_id cmd);
		bool empty() const;

	private:
		vector<buffer_request> _requests = {};
	};
} // namespace Lina

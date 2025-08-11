// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/vector.hpp"
#include "common/size_definitions.hpp"
#include "gfx/common/gfx_constants.hpp"
#include <functional>

namespace SFG
{
	struct texture_buffer;

	class texture_queue
	{
	public:
		typedef std::function<void()> flush_callback;

		struct texture_request
		{
			resource_id		texture		 = 0;
			resource_id		intermediate = 0;
			texture_buffer* buffers		 = nullptr;
			uint8			buffer_count = 0;
		};

		void init();
		void uninit();

		void add_request(const texture_request& req);
		void flush_all(resource_id cmd);
		bool empty() const;
		void subscribe_flush_callback(flush_callback&& cb);

	private:
		vector<texture_request> _requests = {};
		vector<flush_callback>	_callbacks;
	};
} // namespace Lina

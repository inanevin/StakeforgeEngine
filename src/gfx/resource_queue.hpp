// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/vector.hpp"
#include "common/size_definitions.hpp"
#include "gfx/common/gfx_common.hpp"

namespace LinaGX
{
	class CommandStream;
}

namespace Game
{
	class buffer;

	class resource_queue
	{
	public:
		struct texture_request
		{
			texture_buffer* buffers		  = nullptr;
			uint32			hw			  = 0;
			uint32			mip_levels	  = 0;
			uint32			_buffer_start = 0;
		};

		struct buffer_request
		{
			buffer* buffer = nullptr;
		};

		void add_texture_request(texture_buffer* buffers, uint32 mip_levels, uint32 hw);
		void add_buffer_request(const buffer_request& req);
		bool flush_all(LinaGX::CommandStream* cmd);

	private:
		vector<texture_request> _texture_requests = {};
		vector<buffer_request>	_buffer_requests  = {};
		vector<texture_buffer>	_reuse_buffers	  = {};
	};
} // namespace Lina

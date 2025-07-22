// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "gfx/buffer.hpp"

namespace Game
{
	class material
	{
	public:
		// void create(const size_t size, const LinaGX::DescriptorSetDesc& set_desc);
		// void destroy();
		// void update_textures(uint32 binding, const vector<uint32>& hws);
		// void update_samplers(uint32 binding, const vector<uint32>& hws);
		void set_ubo_data(size_t padding, uint8* data, size_t size);

		inline uint16 get_hw() const
		{
			return _hw;
		}

	private:
		buffer _buffer = {};
		uint16 _hw	   = 0;
	};

}

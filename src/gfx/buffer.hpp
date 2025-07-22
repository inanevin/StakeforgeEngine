/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/atomic.hpp"
#include "data/bitmask.hpp"

namespace Game
{
	class buffer
	{
	public:
		enum flags
		{
			bf_staging_only	   = 1 << 0,
			bf_cpu_visible_gpu = 1 << 1,
			bf_resides_in_gpu  = 1 << 2,
			bf_dirty		   = 1 << 3,
		};

		buffer()  = default;
		~buffer() = default;

		void create(uint32 hintFlags, uint32 size, const string& debugName = "GPUBuffer", bool stagingOnly = false);
		void set_data(size_t padding, uint8* data, size_t size);
		void memset_mapped(int32 v);
		// bool copy(LinaGX::CommandStream* stream);
		void destroy();
		// void bind_vertex(LinaGX::CommandStream* stream, uint32 vertexSize);
		// void bind_index(LinaGX::CommandStream* stream, LinaGX::IndexType indexType);

		inline void mark_dirty()
		{
			_buffer_flags.set(bf_dirty);
		}
		inline uint32 get_gpu() const
		{
			return _buffer_flags.is_set(bf_resides_in_gpu) ? _gpu : _staging;
		}
		inline uint32 get_staging() const
		{
			return _staging;
		}
		inline uint8* get_mapped() const
		{
			return _mapped;
		}

		inline uint32 get_size() const
		{
			return _size;
		}

	private:
		static atomic<uint64> _s_used_cpu_visible_mem;
		uint8*				  _mapped		= nullptr;
		uint32				  _staging		= 0;
		uint32				  _gpu			= 0;
		uint32				  _size			= 0;
		bitmask<uint8>		  _buffer_flags = 0;
	};
} // namespace Lina

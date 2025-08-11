// Copyright (c) 2025 Inan Evin

#include "bump_allocator.hpp"
#include "memory/memory.hpp"

namespace SFG
{
	void bump_allocator::init(size_t sz, size_t alignment)
	{

		SFG_ASSERT(sz != 0);
		_size = sz;
		_raw  = SFG_ALIGNED_MALLOC(alignment, sz);
	}

	void bump_allocator::uninit()
	{
		SFG_ALIGNED_FREE(_raw);
		_raw = nullptr;
	}

	void* bump_allocator::allocate(size_t size, size_t alignment)
	{
		SFG_ASSERT(_head + size < _size);

		void*  current_ptr = (void*)((uint8*)_raw + _head);
		size_t space	   = _size - _head;

		void* aligned_ptr = std::align(alignment, size, current_ptr, space);
		if (aligned_ptr == nullptr)
			return nullptr;

		_head = _size - space + size;
		return aligned_ptr;
	}
}

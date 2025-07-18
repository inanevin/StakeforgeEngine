// Copyright (c) 2025 Inan Evin

#include "bump_allocator.hpp"
#include "memory/memory.hpp"

namespace Game
{
	bum_allocator::bum_allocator(size_t sz)
	{
		GAME_ASSERT(sz != 0);
		_size = sz;
		_raw  = GAME_MALLOC(sz);
	}

	bum_allocator::~bum_allocator()
	{
		GAME_FREE(_raw);
	}

	void* bum_allocator::allocate(size_t size, size_t alignment)
	{
		GAME_ASSERT(_head + size < _size);

		void*  current_ptr = (void*)((uint8*)_raw + _head);
		size_t space	   = _size - _head;

		void* aligned_ptr = std::align(alignment, size, current_ptr, space);
		if (aligned_ptr == nullptr) return nullptr;

		_head = _size - space + size;
		return aligned_ptr;
	}
}

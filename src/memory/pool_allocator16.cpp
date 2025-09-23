// Copyright (c) 2025 Inan Evin

#include "pool_allocator16.hpp"

namespace SFG
{

	pool_allocator16::~pool_allocator16()
	{
		if (_raw != nullptr)
			uninit();
	}

	void pool_allocator16::uninit()
	{
#ifdef ENABLE_MEMORY_TRACER
		memory_tracer::get().on_free(_raw);
#endif
		SFG_ASSERT(_raw != nullptr);
		reset();
		SFG_ALIGNED_FREE(_raw);
		_raw = nullptr;
	}

	void pool_allocator16::reset()
	{
		if (_raw)
			SFG_MEMSET(_raw, 0, _item_size_aligned * _item_count + sizeof(uint16) * _item_count + sizeof(uint16) * _item_count + sizeof(uint8) * _item_count);
		_head			   = 0;
		_free_count		   = 0;
		_item_count		   = 0;
		_item_size_aligned = 0;
	}

	bool pool_allocator16::is_valid(pool_handle16 handle) const
	{
		uint16* generations = get_generations();
		return handle.generation == generations[handle.index];
	}

}

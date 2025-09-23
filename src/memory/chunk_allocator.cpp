// Copyright (c) 2025 Inan Evin

#include "chunk_allocator.hpp"
#include "math/math_common.hpp"

#include "memory/memory_tracer.hpp"

namespace SFG
{

	chunk_allocator32::~chunk_allocator32()
	{
		SFG_ASSERT(_raw == nullptr);
		if (_raw != nullptr)
			uninit();
	}

	void chunk_allocator32::init(size_t size)
	{
		const size_t alignment = alignof(uint32);
		SFG_ASSERT(size % alignment == 0);

		const size_t mem_size = ALIGN_UP(size, alignment);
		_raw				  = reinterpret_cast<uint8*>(SFG_ALIGNED_MALLOC(alignment, mem_size));
		_total_size			  = mem_size;

#ifdef ENABLE_MEMORY_TRACER
		memory_tracer::get().on_allocation(_raw, mem_size);
#endif
	}

	void chunk_allocator32::uninit()
	{
#ifdef ENABLE_MEMORY_TRACER
		memory_tracer::get().on_free(_raw);
#endif

		SFG_ASSERT(_raw != nullptr);
		SFG_ALIGNED_FREE(_raw);
		_raw = nullptr;
	}

	void chunk_allocator32::reset()
	{
		_free_chunks.resize(0);
		_head = 0;
	}
}

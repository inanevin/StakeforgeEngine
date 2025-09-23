// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "memory/memory.hpp"
#include "io/assert.hpp"
#include "math/math_common.hpp"

namespace SFG
{
	template <typename T> class pool_allocator_simple
	{
	public:
		inline void init(size_t item_count)
		{
			SFG_ASSERT(_raw == nullptr);
			_item_size_aligned = ALIGN_UP(sizeof(T), alignof(T));
			_raw			   = reinterpret_cast<uint8*>(SFG_ALIGNED_MALLOC(alignof(T), _item_size_aligned * item_count));
			_item_count		   = item_count;

			T* ptr = reinterpret_cast<T*>(_raw);
			for (uint32 i = 0; i < item_count; i++)
				ptr[i] = T();
		}

		inline void reset()
		{
			T* ptr = reinterpret_cast<T*>(_raw);
			for (uint32 i = 0; i < _item_count; i++)
				ptr[i] = T();
		}

		inline void reset(uint32 index)
		{
			T* ptr	   = reinterpret_cast<T*>(_raw);
			ptr[index] = T();
		}

		inline void uninit()
		{
			SFG_ASSERT(_raw != nullptr);
			SFG_ALIGNED_FREE(_raw);
			_raw = nullptr;
		}

		T& get(uint32 index)
		{
			T* t = reinterpret_cast<T*>(_raw + (_item_size_aligned * index));
			return *t;
		}

		const T& get(uint32 index) const
		{
			T* t = reinterpret_cast<T*>(_raw + (_item_size_aligned * index));
			return *t;
		}

	private:
		uint8* _raw				  = nullptr;
		uint32 _item_size_aligned = 0;
		uint32 _item_count		  = 0;
	};

}

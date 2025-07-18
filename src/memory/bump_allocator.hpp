// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "io/assert.hpp"
#include <new>

namespace Game
{
	class bum_allocator
	{
	public:
		bum_allocator(size_t sz);
		~bum_allocator();

		bum_allocator()										 = delete;
		bum_allocator& operator=(const bum_allocator& other) = delete;
		bum_allocator(const bum_allocator& other)			 = delete;

		void* allocate(size_t size, size_t alignment = 1);

		inline void reset()
		{
			_head = 0;
		}

		template <typename T, typename... Args> T* allocate(size_t count, Args&&... args)
		{
			if (count == 0) return nullptr;

			void* ptr	   = allocate(sizeof(T) * count, std::alignment_of<T>::value);
			T*	  arrayPtr = reinterpret_cast<T*>(ptr);
			for (size_t i = 0; i < count; ++i)
				new (&arrayPtr[i]) T(std::forward<Args>(args)...);
			return arrayPtr;
		}

		template <typename T, typename... Args> T* emplace_aux(T firstValue, Args&&... remainingValues)
		{
			uint8* initial_head = (uint8*)_raw + _head;

			// Place the first value in memory
			uint8* current_head = initial_head;
			GAME_MEMCPY(current_head, &firstValue, sizeof(T));
			_head += sizeof(T);
			GAME_ASSERT(_head < _size);

			// Recursively place the remaining values in memory
			if constexpr (sizeof...(remainingValues) > 0) { emplace_aux<T>(remainingValues...); }

			return reinterpret_cast<T*>(initial_head);
		}

	private:
		size_t _size = 0;
		size_t _head = 0;
		void*  _raw	 = nullptr;
	};
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "io/assert.hpp"
#include "memory.hpp"
#include <new>

namespace Game
{
	class bump_allocator
	{
	public:
		void init(size_t sz, size_t alignment);
		void uninit();

		bump_allocator()									   = default;
		bump_allocator& operator=(const bump_allocator& other) = delete;
		bump_allocator(const bump_allocator& other)			   = delete;
		~bump_allocator()
		{
			GAME_ASSERT(_raw == nullptr);
		}

		void* allocate(size_t size, size_t alignment = 1);

		inline void reset()
		{
			_head = 0;
		}

		template <typename T, typename... Args> T* allocate(size_t count, Args&&... args)
		{
			if (count == 0)
				return nullptr;

			void* ptr	   = allocate(sizeof(T) * count, std::alignment_of<T>::value);
			T*	  arrayPtr = reinterpret_cast<T*>(ptr);
			for (size_t i = 0; i < count; ++i)
				new (&arrayPtr[i]) T(std::forward<Args>(args)...);
			return arrayPtr;
		}

		template <typename T, typename... Args> T* emplace_aux(T firstValue, Args&&... remainingValues)
		{
			uint8* initial_head = (uint8*)_raw + _head;

			uint8* current_head = initial_head;
			GAME_MEMCPY(current_head, &firstValue, sizeof(T));
			_head += sizeof(T);
			GAME_ASSERT(_head < _size);

			if constexpr (sizeof...(remainingValues) > 0)
			{
				emplace_aux<T>(remainingValues...);
			}

			return reinterpret_cast<T*>(initial_head);
		}

		inline size_t get_size() const
		{
			return _size;
		}

		inline size_t get_head() const
		{
			return _head;
		}

	private:
		size_t _size = 0;
		size_t _head = 0;
		void*  _raw	 = nullptr;
	};
}

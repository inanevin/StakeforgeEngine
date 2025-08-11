// Copyright (c) 2025 Inan Evin

#pragma once

#undef min
#undef max

#include <stdlib.h>
#include <new>
#include <limits>

namespace SFG
{
	template <class T> struct malloc_allocator_stl
	{
		typedef T value_type;

		malloc_allocator_stl() = default;

		template <class U> constexpr malloc_allocator_stl(const malloc_allocator_stl<U>&) noexcept
		{
		}

		[[nodiscard]] T* allocate(std::size_t n)
		{
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
				throw std::bad_array_new_length();

			if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
			{
				return p;
			}

			throw std::bad_alloc();
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			std::free(p);
		}
	};

	template <class T, class U> bool operator==(const malloc_allocator_stl<T>&, const malloc_allocator_stl<U>&)
	{
		return true;
	}

	template <class T, class U> bool operator!=(const malloc_allocator_stl<T>&, const malloc_allocator_stl<U>&)
	{
		return false;
	}
}

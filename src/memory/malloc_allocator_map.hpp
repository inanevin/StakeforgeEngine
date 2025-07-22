// Copyright (c) 2025 Inan Evin

#pragma once

#include <stdlib.h>
#include <new>
#include <limits>

#undef max

namespace Game
{
	template <class T> struct malloc_allocator_map
	{
		typedef size_t	  size_type;
		typedef ptrdiff_t difference_type;
		typedef T*		  pointer;
		typedef const T*  const_pointer;
		typedef T&		  reference;
		typedef const T&  const_reference;
		typedef T		  value_type;

		template <class U> struct rebind
		{
			typedef malloc_allocator_map<U> other;
		};
		malloc_allocator_map() throw()
		{
		}
		malloc_allocator_map(const malloc_allocator_map&) throw()
		{
		}

		template <class U> malloc_allocator_map(const malloc_allocator_map<U>&) throw()
		{
		}

		~malloc_allocator_map() throw()
		{
		}

		pointer address(reference x) const
		{
			return &x;
		}
		const_pointer address(const_reference x) const
		{
			return &x;
		}

		pointer allocate(size_type s, void const* = 0)
		{
			if (0 == s)
				return NULL;
			pointer temp = (pointer)malloc(s * sizeof(T));
			if (temp == NULL)
				throw std::bad_alloc();
			return temp;
		}

		void deallocate(pointer p, size_type)
		{
			free(p);
		}

		size_type max_size() const throw()
		{
			return std::numeric_limits<size_t>::max() / sizeof(T);
		}

		void construct(pointer p, const T& val)
		{
			new ((void*)p) T(val);
		}

		void destroy(pointer p)
		{
			p->~T();
		}
	};

}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "memory.hpp"
#include "io/assert.hpp"
#include "data/vector.hpp"
#include "data/vector_util.hpp"
#include <cstring>

namespace SFG
{
	template <size_t CAPACITY> class text_allocator
	{

	private:
		struct allocation
		{
			char*  ptr	= nullptr;
			size_t size = 0;
		};

	public:
		text_allocator() : _head(0)
		{
			SFG_MEMSET(_raw, 0, CAPACITY);
			_free_list.reserve(CAPACITY);
		}

		const char* allocate(size_t len)
		{
			auto it = vector_util::find_if(_free_list, [len](const allocation& alloc) -> bool { return alloc.size > len; });
			if (it != _free_list.end())
			{
				allocation& free = *it;
				if (free.size == len)
				{
					_free_list.erase(it);
					return free.ptr;
				}
				else
				{
					free.size -= len;
					return free.ptr;
				}
			}

			SFG_ASSERT(_head + len + 1 < CAPACITY);

			char* allocated = &_raw[_head];
			_head += len + 1;

			return allocated;
		}

		const char* allocate(const char* text)
		{
			if (!text)
				return nullptr;

			const size_t len = strlen(text);

			auto it = vector_util::find_if(_free_list, [len](const allocation& alloc) -> bool { return alloc.size > len; });
			if (it != _free_list.end())
			{
				allocation& free = *it;
				if (free.size == len)
				{
					std::strcpy(free.ptr, text);
					_free_list.erase(it);
					return free.ptr;
				}
				else
				{
					free.size -= len;
					std::strcpy(free.ptr, text);
					return free.ptr;
				}
			}

			SFG_ASSERT(_head + len + 1 < CAPACITY);

			char* allocated = &_raw[_head];
			std::strcpy(allocated, text);
			_head += len + 1;

			return allocated;
		}

		void deallocate(char* ptr)
		{
			_free_list.push_back({
				.ptr  = ptr,
				.size = strlen(ptr),
			});
		}

		inline constexpr size_t get_capacity() const
		{
			return CAPACITY;
		}

		inline constexpr size_t get_head() const
		{
			return _head;
		}

		inline char* get_raw() const
		{
			return _raw;
		}

		inline void reset()
		{
			_free_list.resize(0);
			_alloc_count = 0;
			_head		 = 0;
		}

	private:
		char			   _raw[CAPACITY];
		vector<allocation> _free_list;
		size_t			   _alloc_count = 0;
		size_t			   _head		= 0;
	};

}

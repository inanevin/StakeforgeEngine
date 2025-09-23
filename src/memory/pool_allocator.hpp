// Copyright (c) 2025 Inan Evin

#pragma once

#include "pool_handle.hpp"
#include "io/assert.hpp"
#include "memory.hpp"
#include "data/vector.hpp"
#include <new>

namespace SFG
{

	template <typename T, typename U, int N> struct pool_allocator
	{
		T		  data[N];
		vector<U> free_list = {};
		U		  head		= 0;

		inline U add()
		{
			if (!free_list.empty())
			{
				const U id = free_list.back();
				free_list.pop_back();
				return id;
			}

			const U id = head;
			head++;
			SFG_ASSERT(head < N);
			return id;
		}

		void remove(U id)
		{
			free_list.push_back(id);
			data[id] = T();
		}

		T& get(U id)
		{
			SFG_ASSERT(id < N);
			return data[id];
		}

		const T& get(U id) const
		{
			SFG_ASSERT(id < N);
			return data[id];
		}

		inline void verify_uninit()
		{
			SFG_ASSERT(static_cast<U>(free_list.size()) == head);
		}

		void reset()
		{
			for (int i = 0; i < N; i++)
				data[i] = {};

			free_list.resize(0);
			head = 0;
		}
	};

}

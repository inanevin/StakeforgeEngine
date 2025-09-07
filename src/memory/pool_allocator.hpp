// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
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
	};

	template <typename T> struct pool_handle
	{
		T id;
		T generation;
	};

	template <typename T, typename U, int N> struct pool_allocator_gen
	{
		struct slot
		{
			T	 value;
			U	 generation = 0;
			bool active		= false;
		};

		slot	  data[N];
		vector<U> free_list = {};
		U		  head		= 0;

		pool_handle<U> add()
		{
			U id;
			if (!free_list.empty())
			{
				id = free_list.back();
				free_list.pop_back();
			}
			else
			{
				id = head;
				head++;
				SFG_ASSERT(head <= N);
			}

			data[id].active = true;
			return {id, data[id].generation};
		}

		void remove(pool_handle<U> h)
		{
			if (!is_valid(h))
				return;

			data[h.id].active = false;
			data[h.id].value  = T();
			data[h.id].generation++;
			free_list.push_back(h.id);
		}

		bool is_valid(pool_handle<U> h) const
		{
			if (h.id >= head)
				return false;
			const slot& s = data[h.id];
			return s.active && s.generation == h.generation;
		}

		T& get(pool_handle<U> h)
		{
			SFG_ASSERT(is_valid(h));
			return data[h.id].value;
		}

		const T& get(pool_handle<U> h) const
		{
			SFG_ASSERT(is_valid(h));
			return data[h.id].value;
		}

		inline void verify_uninit()
		{
			SFG_ASSERT(free_list.size() == head);
		}

		//-----------------------------------
		// Iterator definition
		//-----------------------------------

		template <bool IsConst> struct base_iterator
		{
			using slot_type			= slot;
			using value_type		= T;
			using difference_type	= std::ptrdiff_t;
			using reference			= std::conditional_t<IsConst, const T&, T&>;
			using pointer			= std::conditional_t<IsConst, const T*, T*>;
			using iterator_category = std::forward_iterator_tag;
			using slot_pointer		= std::conditional_t<IsConst, const slot_type*, slot_type*>;
			slot_pointer current;
			slot_pointer end;

			base_iterator(slot_pointer start, slot_pointer end) noexcept : current(start), end(end)
			{
				while (current != end && !current->active)
					++current;
			}

			reference operator*() const
			{
				return current->value;
			}
			pointer operator->() const
			{
				return &current->value;
			}

			base_iterator& operator++()
			{
				do
				{
					++current;
				} while (current != end && !current->active);
				return *this;
			}

			base_iterator operator++(int)
			{
				base_iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const base_iterator& other) const
			{
				return current == other.current;
			}
			bool operator!=(const base_iterator& other) const
			{
				return current != other.current;
			}
		};

		using iterator		 = base_iterator<false>;
		using const_iterator = base_iterator<true>;

		//-----------------------------------
		// begin/end
		//-----------------------------------
		iterator begin()
		{
			return iterator(data, data + head);
		}
		iterator end()
		{
			return iterator(data + head, data + head);
		}

		const_iterator begin() const
		{
			return const_iterator(data, data + head);
		}
		const_iterator end() const
		{
			return const_iterator(data + head, data + head);
		}

		const_iterator cbegin() const
		{
			return const_iterator(data, data + head);
		}
		const_iterator cend() const
		{
			return const_iterator(data + head, data + head);
		}
	};

}

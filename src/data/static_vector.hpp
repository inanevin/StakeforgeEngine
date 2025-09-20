// Copyright (c) 2025 Inan Evin

#pragma once

#include "io/assert.hpp"
#include <cstddef>
#include <stdexcept>
#include <algorithm>

namespace SFG
{
	template <typename T, int N> class static_vector
	{
	public:
		using value_type	  = T;
		using size_type		  = std::size_t;
		using reference		  = value_type&;
		using const_reference = const value_type&;
		using iterator		  = T*;
		using const_iterator  = const T*;

		static constexpr size_type capacity = N;

		static_vector() : _head(0)
		{
		}

		reference operator[](size_type index)
		{
			return _data[index];
		}

		const_reference operator[](size_type index) const
		{
			return _data[index];
		}

		reference at(size_type index)
		{
			if (index >= size())
				throw std::out_of_range("static_vector::at");
			return _data[index];
		}

		const_reference at(size_type index) const
		{
			if (index >= size())
				throw std::out_of_range("static_vector::at");
			return _data[index];
		}

		reference front()
		{
			return _data[0];
		}

		const_reference front() const
		{
			return _data[0];
		}

		reference back()
		{
			return _data[_head - 1];
		}
		const_reference back() const
		{
			return _data[_head - 1];
		}

		iterator begin()
		{
			return _data;
		}
		const_iterator begin() const
		{
			return _data;
		}
		const_iterator cbegin() const
		{
			return _data;
		}

		iterator end()
		{
			return _data + _head;
		}
		const_iterator end() const
		{
			return _data + _head;
		}
		const_iterator cend() const
		{
			return _data + _head;
		}

		size_type size() const
		{
			return _head;
		}

		bool empty() const
		{
			return _head == 0;
		}

		bool full() const
		{
			return _head == N;
		}

		void push_back(const T& value)
		{
			SFG_ASSERT(!full());
			_data[_head++] = value;
		}

		void push_back(T&& value)
		{
			SFG_ASSERT(!full());
			_data[_head++] = std::move(value);
		}

		void pop_back()
		{
			if (!empty())
				--_head;
		}

		void clear()
		{
			_head = 0;
		}

		void resize(size_t sz)
		{
			SFG_ASSERT(sz < capacity);
			_head = sz;
		}

		void remove(const T& value)
		{
			auto end = begin() + _head;
			auto it	 = std::find(begin(), end, value);
			SFG_ASSERT(it != end);

			const size_t idx = it - begin();
			remove_index(idx);
		}

		template <typename Pred> void remove_if(Pred pred)
		{
			auto end = begin() + _head;
			auto it	 = std::find_if(begin(), end, pred);
			SFG_ASSERT(it != end);

			const size_t idx = it - begin();
			remove_index(idx);
		}

		void remove_index(size_t idx)
		{
			SFG_ASSERT(idx < _head);
			for (size_t i = idx; i < _head - 1; i++)
				_data[i] = _data[i + 1];
			_head--;
		}

		T* data()
		{
			return _data;
		}

	private:
		T	   _data[N];
		size_t _head;
	};
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "chunk_handle.hpp"
#include "io/assert.hpp"
#include "memory.hpp"
#include "data/vector.hpp"
#include "data/vector_util.hpp"

namespace SFG
{

	template <typename T, T N> class chunk_allocator
	{
	public:
		chunk_allocator()
		{
			SFG_MEMSET(_data, 0, N);
			_free_chunks.reserve(N / 8);
		}

		inline chunk_handle<T> allocate(size_t size)
		{
			const T sz = static_cast<T>(size);

			if (!_free_chunks.empty())
			{
				auto it = vector_util::find_if(_free_chunks, [sz](const chunk_handle<T>& handle) -> bool { return handle.size > sz; });
				if (it != _free_chunks.end())
				{
					chunk_handle<T> handle = *it;
					_free_chunks.erase(it);

					const T remaining = handle.size - sz;

					const chunk_handle<T> retVal = {
						.head = handle.head,
						.size = sz,
					};

					if (remaining > 0)
					{
						const chunk_handle<T> ch = {
							.head = static_cast<T>(handle.head + sz),
							.size = remaining,
						};

						_free_chunks.push_back(ch);
					}

					return retVal;
				}
			}

			SFG_ASSERT(_head + sz < N);
			const chunk_handle<T> retVal = {
				.head = _head,
				.size = sz,
			};
			_head += sz;
			return retVal;
		}

		inline void free(chunk_handle<T> handle)
		{
			_free_chunks.push_back(handle);
			SFG_MEMSET(_data + handle.head, 0, handle.size);
		}

		inline uint8* get(T index)
		{
			return &_data[index];
		}

	private:
		uint8					_data[N];
		vector<chunk_handle<T>> _free_chunks;
		T						_head = 0;
	};

	template <uint16 N> using chunk_allocator16 = chunk_allocator<uint16, N>;
	template <uint32 N> using chunk_allocator32 = chunk_allocator<uint32, N>;

}

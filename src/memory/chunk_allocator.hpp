// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "chunk_handle.hpp"
#include "io/assert.hpp"
#include "data/vector.hpp"
#include "data/vector_util.hpp"
#include "math/math_common.hpp"
#include "memory.hpp"

namespace SFG
{

	class chunk_allocator32
	{
	public:
		~chunk_allocator32();

		void init(size_t total_size);
		void uninit();
		void reset();

		template <typename T> inline chunk_handle32 allocate(size_t count)
		{
			SFG_ASSERT(count != 0);

			const size_t item_alignment	  = alignof(T);
			const size_t padded_item_size = ALIGN_UP(sizeof(T), item_alignment);
			const uint32 requested_size	  = static_cast<uint32>(padded_item_size * count);

			if (!_free_chunks.empty())
			{
				for (auto it = _free_chunks.begin(); it != _free_chunks.end(); ++it)
				{
					const chunk_handle32 chunk = *it;

					const uint32 aligned_head		 = ALIGN_UP(chunk.head, item_alignment);
					const uint32 aligned_size_needed = (aligned_head - chunk.head) + requested_size;

					if (chunk.size >= aligned_size_needed)
					{
						_free_chunks.erase(it);

						const chunk_handle32 allocated_chunk = {.head = aligned_head, .size = requested_size};

						// pre-chunk split.
						if (aligned_head > chunk.head)
							_free_chunks.push_back({.head = chunk.head, .size = aligned_head - chunk.head});

						// post-chunk split
						const uint32 remaining_size = chunk.size - aligned_size_needed;
						if (remaining_size > 0)
							_free_chunks.push_back({.head = allocated_chunk.head + allocated_chunk.size, .size = remaining_size});

						return allocated_chunk;
					}
				}
			}

			const uint32 current_aligned_head = ALIGN_UP(_head, item_alignment);
			const uint32 needed_size		  = (current_aligned_head - _head) + requested_size;

			SFG_ASSERT(_head + needed_size <= _total_size);

			const chunk_handle32 ret_val = {
				.head = current_aligned_head,
				.size = requested_size,
			};

			T* ptr = reinterpret_cast<T*>(_raw + ret_val.head);
			for (size_t i = 0; i < count; i++)
			{
				ptr[i] = T();
			}

			_head += needed_size;
			return ret_val;
		}

		inline void free(chunk_handle32 handle)
		{
			SFG_ASSERT(handle.size != 0);

			SFG_MEMSET(_raw + handle.head, 0, handle.size);

			// sorted by head address.
			auto it = std::lower_bound(_free_chunks.begin(), _free_chunks.end(), handle, [](const chunk_handle32& a, const chunk_handle32& b) { return a.head < b.head; });

			bool was_merged = false;

			// Check if the new chunk can be merged with the one before it.
			if (it != _free_chunks.begin())
			{
				auto prev_it = it - 1;
				if (prev_it->head + prev_it->size == handle.head)
				{
					// merge prev
					prev_it->size += handle.size;
					handle	   = *prev_it; // update for next merge check
					it		   = prev_it + 1;
					was_merged = true;
				}
			}

			// merge with next
			if (it != _free_chunks.end() && handle.head + handle.size == it->head)
			{
				it->head = handle.head;
				it->size += handle.size;
			}
			else
			{
				// no merge
				if (!was_merged)
					_free_chunks.insert(it, handle);
			}
		}

		template <typename T> T* get(chunk_handle32 handle)
		{
			SFG_ASSERT(handle.size != 0);
			return reinterpret_cast<T*>(_raw + handle.head);
		}

		inline uint8* get(uint32 index)
		{
			return _raw + index;
		}

		inline uint32 get_current() const
		{
			return _head;
		}

	private:
		uint8*				   _raw = nullptr;
		vector<chunk_handle32> _free_chunks;
		uint32				   _head	   = 0;
		uint32				   _total_size = 0;
	};

}

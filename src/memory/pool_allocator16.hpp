// Copyright (c) 2025 Inan Evin

#pragma once

#include "pool_handle.hpp"
#include "io/assert.hpp"
#include "memory/memory.hpp"
#include "memory/memory_tracer.hpp"

namespace SFG
{
	class pool_allocator16
	{
	public:
		~pool_allocator16();

		template <typename T> inline void init(size_t item_count)
		{
			const size_t item_size		= sizeof(T);
			const size_t item_alignment = alignof(T);
			SFG_ASSERT(_raw == nullptr);
			SFG_ASSERT(item_size % item_alignment == 0);

			auto align = [](size_t sz, size_t alignment) -> size_t { return (sz + alignment + 1) & ~(alignment - 1); };

			const size_t padded_item_size = align(item_size, item_alignment);
			const size_t sz_items		  = padded_item_size * item_count;
			_item_size_aligned			  = padded_item_size;

			const size_t sz_free_indices = align(sizeof(uint16) * item_count, alignof(uint16));
			const size_t sz_generations	 = align(sizeof(uint16) * item_count, alignof(uint16));
			const size_t sz_actives		 = align(sizeof(uint8) * item_count, alignof(uint16));
			const size_t total_size		 = sz_items + sz_free_indices + sz_generations + sz_actives;
			_raw						 = reinterpret_cast<uint8*>(SFG_ALIGNED_MALLOC(item_alignment, total_size));

#ifdef ENABLE_MEMORY_TRACER
			memory_tracer::get().on_allocation(_raw, total_size);
#endif
			_item_count = static_cast<uint16>(item_count);

			if (_raw != nullptr)
			{
				T* ptr = reinterpret_cast<T*>(_raw);

				uint16* generations	 = get_generations();
				uint16* free_indices = get_free_indices();
				uint8*	actives		 = get_actives();

				for (uint16 i = 0; i < _item_count; i++)
				{
					free_indices[i] = 0;
					generations[i]	= 1;
					actives[i]		= 0;
					ptr[i]			= T();
				}
			}
		}

		void uninit();
		void reset();
		bool is_valid(pool_handle16 handle) const;

		template <typename T> inline void free(pool_handle16 handle)
		{
			SFG_ASSERT(is_valid(handle));

			T& t = get<T>(handle);
			t.~T();

			uint16* generations	 = get_generations();
			uint16* free_indices = get_free_indices();
			uint8*	actives		 = get_actives();

			actives[handle.index] = 0;
			generations[handle.index]++;
			free_indices[_free_count] = handle.index;
			_free_count++;
		}

		template <typename T> pool_handle16 allocate()
		{
			const size_t aligned = (sizeof(T) + alignof(T) + 1) & ~(alignof(T) - 1);
			SFG_ASSERT(aligned == static_cast<size_t>(_item_size_aligned));

			uint16 index = 0;

			if (_free_count != 0)
			{
				uint16* free_indices = get_free_indices();
				index				 = free_indices[_free_count - 1];
				_free_count--;
			}
			else
			{
				index = _head;
				_head++;
				SFG_ASSERT(_head <= _item_count);
			}

			uint8* actives = get_actives();
			actives[index] = 1;

			T& data = *(reinterpret_cast<T*>(_raw + static_cast<size_t>(_item_size_aligned) * index));
			data	= T();

			uint16* generations = get_generations();
			return {.generation = generations[index], .index = index};
		}

		template <typename T> inline T& get(pool_handle16 handle) const
		{
			SFG_ASSERT(is_valid(handle));
			const size_t pos  = sizeof(T) * handle.index;
			T*			 item = reinterpret_cast<T*>(_raw + pos);
			return *item;
		}

		struct handle_iterator
		{
			uint16* gens	= nullptr;
			uint8*	actives = nullptr;
			uint16	current = 0;
			uint16	end		= 0;

			handle_iterator(uint16 _start, uint16 _end, uint16* _gens, uint8* _actives) : current(_start), gens(_gens), actives(_actives), end(_end)
			{
				while (current != end && actives[current] == 0)
					++current;
			}

			pool_handle16 operator*() const
			{
				return {.generation = gens[current], .index = current};
			}

			handle_iterator& operator++()
			{
				do
				{
					++current;
				} while (current != end && actives[current] == 0);
				return *this;
			}

			bool operator==(const handle_iterator& other) const
			{
				return current == other.current;
			}

			bool operator!=(const handle_iterator& other) const
			{
				return current != other.current;
			}
		};

		handle_iterator begin() const
		{
			return handle_iterator(0, _head, get_generations(), get_actives());
		}

		handle_iterator end() const
		{
			return handle_iterator(_head, _head, get_generations(), get_actives());
		}

		inline uint8* get_raw() const
		{
			return _raw;
		}

	private:
		uint16* get_free_indices() const
		{
			return reinterpret_cast<uint16*>(_raw + _item_count * _item_size_aligned + sizeof(uint16) * _item_count);
		}
		uint16* get_generations() const
		{
			return reinterpret_cast<uint16*>(_raw + _item_count * _item_size_aligned);
		}
		uint8* get_actives() const
		{
			return reinterpret_cast<uint8*>(_raw + _item_count * _item_size_aligned + sizeof(uint16) * _item_count + sizeof(uint16) * _item_count);
		}

	private:
		uint8* _raw				  = nullptr;
		uint16 _head			  = 0;
		uint16 _free_count		  = 0;
		uint16 _item_count		  = 0;
		uint16 _item_size_aligned = 0;
	};
}

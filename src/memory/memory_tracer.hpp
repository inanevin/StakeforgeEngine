
// Copyright (c) 2025 Inan Evin

#pragma once

#ifndef NDEBUG
#define ENABLE_MEMORY_TRACER
#endif

#ifdef ENABLE_MEMORY_TRACER
#include "data/hash_map.hpp"
#include "data/mutex.hpp"
#include "malloc_allocator_map.hpp"
#include "malloc_allocator_stl.hpp"

namespace Game
{
#define MEMORY_STACK_TRACE_SIZE 50

	struct memory_track
	{
		void*		   ptr							  = nullptr;
		size_t		   size							  = 0;
		unsigned short stack_size					  = 0;
		void*		   stack[MEMORY_STACK_TRACE_SIZE] = {};
	};

	typedef phmap::flat_hash_map<void*, memory_track, phmap::priv::hash_default_hash<void*>, phmap::priv::hash_default_eq<void*>, malloc_allocator_map<void*>> alloc_map;

	struct memory_category
	{
		alloc_map	allocations;
		const char* name	   = nullptr;
		size_t		total_size = 0;
	};

	class memory_tracer
	{
	public:
		static memory_tracer& get()
		{
			static memory_tracer instance;
			return instance;
		}

		void on_allocation(void* ptr, size_t sz);
		void on_free(void* ptr);

		void push_category(const char* name);
		void pop_category();

	protected:
		void destroy();

	private:
		memory_tracer() = default;
		~memory_tracer()
		{
			destroy();
		}

		void capture_trace(memory_track& track);
		void check_leaks();

	private:
		template <typename T> using vector_malloc = std::vector<T, malloc_allocator_stl<T>>;

		mutex						   _category_mtx;
		vector_malloc<memory_category> _categories;
		alloc_map					   _leaks;
	};

#define PUSH_MEMORY_CATEGORY(NAME) Game::memory_tracer::get().push_category(NAME)
#define POP_MEMORY_CATEGORY()	   Game::memory_tracer::get().pop_category()
}

#else

#define PUSH_MEMORY_CATEGORY(NAME)
#define POP_MEMORY_CATEGORY()
#endif
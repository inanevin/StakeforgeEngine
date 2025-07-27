
// Copyright (c) 2025 Inan Evin

#pragma once

#ifndef NDEBUG
#define ENABLE_MEMORY_TRACER
#endif

#ifdef ENABLE_MEMORY_TRACER
#include "data/hash_map.hpp"
#include "data/mutex.hpp"
#include "common/size_definitions.hpp"
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
	template <typename T> using vector_malloc = std::vector<T, malloc_allocator_stl<T>>;

	struct memory_category
	{
		const char* name	   = nullptr;
		size_t		total_size = 0;
		uint8		id		   = 0;
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

		mutex& get_category_mtx()
		{
			return _category_mtx;
		}

		const vector_malloc<memory_category>& get_categories() const
		{
			return _categories;
		}

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
		mutex						   _category_mtx;
		vector_malloc<memory_category> _categories;
		vector_malloc<uint8>		   _category_ids;
		alloc_map					   _allocations;

		uint8		 _current_active_category = 0;
		static uint8 s_category_counter;
	};

#define PUSH_MEMORY_CATEGORY(NAME) Game::memory_tracer::get().push_category(NAME)
#define POP_MEMORY_CATEGORY()	   Game::memory_tracer::get().pop_category()
}

#else

#define PUSH_MEMORY_CATEGORY(NAME)
#define POP_MEMORY_CATEGORY()
#define CHECK_LEAKS()
#endif
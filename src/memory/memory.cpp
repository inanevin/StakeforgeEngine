// Copyright (c) 2025 Inan Evin

#include "memory.hpp"
#include "memory_tracer.hpp"

void* operator new(std::size_t size)
{
	void* ptr = malloc(size);

#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_allocation(ptr, size);
#endif
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = malloc(size);
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_allocation(ptr, size);
#endif
	return ptr;
}

void operator delete[](void* ptr)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}

void operator delete(void* ptr)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}

void operator delete(void* ptr, size_t sz)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}
void operator delete[](void* ptr, std::size_t sz)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
#ifdef ENABLE_MEMORY_TRACER
	Game::memory_tracer::get().on_free(ptr);
#endif
	free(ptr);
}

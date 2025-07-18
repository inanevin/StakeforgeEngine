// Copyright (c) 2025 Inan Evin

#pragma once
#include "common/size_definitions.hpp"

namespace LinaGX
{
	class CommandStream;
}

namespace Game
{
	class gfx_semaphore
	{
	public:
		gfx_semaphore(){};
		gfx_semaphore(uint16 semaphore) : _semaphore(semaphore){};

		inline uint64* value_ptr()
		{
			return &_value;
		}

		inline uint64 value() const
		{
			return _value;
		}

		inline uint16* semaphore_ptr()
		{
			return &_semaphore;
		}

		inline uint16 semaphore() const
		{
			return _semaphore;
		}

		inline bool is_modified() const
		{
			return _modified;
		}

		inline void increment()
		{
			_value++;
			_modified = true;
		}

		inline void reset_modified()
		{
			_modified = false;
		}

	private:
		uint16 _semaphore = 0;
		uint64 _value	  = 0;
		bool   _modified  = false;
	};
} // namespace Lina

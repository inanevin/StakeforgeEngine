// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"

namespace Game
{
	template <typename T> class bitmask
	{
	public:
		bitmask()  = default;
		~bitmask() = default;

		bitmask(T m) : _mask(m){};

		inline bool is_set(T m) const
		{
			return (_mask & m) != 0;
		}

		inline bool is_all_set(T bits) const
		{
			return (_mask & bits) == bits;
		}

		inline void set(T m)
		{
			_mask |= m;
		}

		inline void set(T m, bool isSet)
		{
			if (isSet)
				_mask |= m;
			else
				_mask &= ~m;
		}

		inline void remove(T m)
		{
			_mask &= ~m;
		}

		inline T value() const
		{
			return _mask;
		}

	private:
		T _mask = 0;
	};

	typedef bitmask<uint8>	bitmask8;
	typedef bitmask<uint16> bitmask16;
	typedef bitmask<uint32> bitmask32;
}

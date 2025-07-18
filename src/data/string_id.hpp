// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include <string>

// Headers here.
namespace Game
{
	typedef uint32 StringId;
	typedef uint32 TypeId;
	typedef uint64 Guid;

	// https://gist.github.com/hwei/1950649d523afd03285c
	class FnvHash
	{
		static const unsigned int								FNV_PRIME	 = 16777619u;
		static const unsigned int								OFFSET_BASIS = 2166136261u;
		template <unsigned int N> static constexpr unsigned int fnv_hash_const(const char (&str)[N], unsigned int I = N)
		{
			return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
		}
		static unsigned int fnv_hash(const char* str)
		{
			const size_t length = strlen(str) + 1;
			unsigned int hash	= OFFSET_BASIS;
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *str++;
				hash *= FNV_PRIME;
			}
			return hash;
		}
		struct Wrapper
		{
			Wrapper(const char* str) : str(str)
			{
			}
			const char* str;
		};

		// static constexpr unsigned int fnvHashConstexpr(const char* str, std::size_t len, unsigned int hash = OFFSET_BASIS)
		// {
		//     return len == 0 ? hash : fnvHashConstexpr(str + 1, len - 1, (hash ^ *str) * FNV_PRIME);
		// }

		static constexpr unsigned int fnv_hash_constexpr(const char* str)
		{
			const size_t length = std::char_traits<char>::length(str) + 1;
			unsigned int hash	= OFFSET_BASIS;
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *str++;
				hash *= FNV_PRIME;
			}
			return hash;
		}

		unsigned int hash_value;

	public:
		// calulate in run-time
		FnvHash(Wrapper wrapper) : hash_value(fnv_hash(wrapper.str))
		{
		}

		//// calulate in compile-time
		// template <unsigned int N>
		// constexpr FnvHash(const char (&str)[N])
		//     : hash_value(fnvHashConst(str))
		//{
		// }

		constexpr FnvHash(const char* str) : hash_value(fnv_hash_constexpr(str))
		{
		}

		// output result
		constexpr operator unsigned int() const
		{
			return this->hash_value;
		}
	};

	constexpr StringId operator"" _hs(const char* str, std::size_t) noexcept
	{
		return FnvHash(str);
	}

#define TO_SID(X)  FnvHash(X.c_str())
#define TO_SIDC(X) FnvHash(X)

}

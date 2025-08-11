// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef SFG_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 28251)
#pragma warning(disable : 6001)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <bit>
#include <algorithm>
#include <array>

namespace SFG
{
	class endianness
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <param name=""></param>
		template <typename T> static inline void swap_endian(T& val, typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type = nullptr)
		{
			union U {
				T									val;
				std::array<std::uint8_t, sizeof(T)> raw;
			} src, dst;

			src.val = val;
			std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
			val = dst.val;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static inline bool should_swap()
		{
			return std::endian::native == std::endian::big;
		}
	};

}

#ifdef SFG_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
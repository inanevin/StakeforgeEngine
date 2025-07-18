// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/vector.hpp"
#include "data/hash_map.hpp"

namespace Game
{
	namespace
	{
		template <typename T> struct is_hashmap : std::false_type
		{
		};

		template <typename U, typename V> struct is_hashmap<hash_map<U, V>> : std::true_type
		{
		};

		template <typename U, typename V> struct is_hashmap<const hash_map<U, V>> : std::true_type
		{
		};

		template <typename T> inline constexpr bool is_hashmap_v = is_hashmap<T>::value;

		template <typename T> struct is_vector : std::false_type
		{
		};

		template <typename U> struct is_vector<std::vector<U>> : std::true_type
		{
		};

		template <typename U> struct is_vector<const std::vector<U>> : std::true_type
		{
		};

		template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

	}
}

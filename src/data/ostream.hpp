// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "serialization/endianness.hpp"
#include "data/vector.hpp"
#include "data/string.hpp"
#include "memory/memory.hpp"
#include "data/hash_map.hpp"
#include "io/assert.hpp"
#include "common/types.hpp"
#include <fstream>

namespace Game
{
	template <typename Stream, typename Key, typename Value> void serialize_hash_map(Stream& stream, const hash_map<Key, Value>& map)
	{
		stream << static_cast<uint32_t>(map.size());
		for (const auto& [key, value] : map)
		{
			stream << key << value;
		}
	}

	template <typename Stream, typename U> void serialize_vector(Stream& stream, vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->serialize(stream);
		}
		else
		{
			for (auto& item : vec)
				stream << item;
		}
	}

	template <typename Stream, typename U> void serialize_vector(Stream& stream, const vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->serialize(stream);
		}
		else
		{
			for (const auto& item : vec)
				stream << item;
		}
	}

	class ostream
	{
	public:
		void create(size_t size);
		void destroy();
		void write_to_ofstream(std::ofstream& stream);
		void write_raw_endian_safe(const uint8* ptr, size_t size);
		void write_raw(const uint8* ptr, size_t size);

		template <typename T> void write(T& t)
		{
			if (_data == nullptr)
				create(sizeof(T));

			uint8* ptr	= (uint8*)&t;
			size_t size = sizeof(T);

			check_grow(size);
			SFG_MEMCPY(&_data[_current_size], ptr, size);
			_current_size += size;
		}

		inline size_t get_size() const
		{
			return _current_size;
		}
		inline uint8* get_raw() const
		{
			return _data;
		}

		inline void shrink(size_t size)
		{
			_current_size = size;
		}

	private:
		void check_grow(size_t sz);

	private:
		uint8* _data		 = nullptr;
		size_t _current_size = 0;
		size_t _total_size	 = 0;
	};

	template <typename T> ostream& operator<<(ostream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			auto copy = const_cast<typename std::remove_const<T>::type&>(val);
			if (endianness::should_swap())
				endianness::swap_endian(copy);
			stream.write<T>(copy);
		}
		else if constexpr (std::is_same_v<T, string> || std::is_same_v<T, const string>)
		{
			const uint32 sz = static_cast<uint32>(val.size());
			stream << sz;
			stream.write_raw_endian_safe((uint8*)val.data(), val.size());
		}
		else if constexpr (std::is_enum_v<T>)
		{
			const uint8 u8 = static_cast<uint8>(val);
			stream << u8;
		}
		else if constexpr (is_vector_v<T>)
		{
			serialize_vector(stream, val);
		}
		else if constexpr (is_hashmap_v<T>)
		{
			using KeyType	= typename T::key_type;
			using ValueType = typename T::mapped_type;
			serialize_hash_map(stream, val);
		}
		else if constexpr (std::is_class_v<T>)
		{
			// Handle custom classes or structs
			val.serialize(stream);
		}
		else
		{
			SFG_ASSERT(false, "");
		}

		return stream;
	}

}

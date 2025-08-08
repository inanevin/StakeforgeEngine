// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "serialization/endianness.hpp"
#include "memory/memory.hpp"
#include "data/string.hpp"
#include "common/types.hpp"
#include "io/assert.hpp"
#include <fstream>

namespace Game
{

	template <typename Stream, typename Key, typename Value> void deserialize_hash_map(Stream& stream, hash_map<Key, Value>& map)
	{
		uint32_t size = 0;
		stream >> size;
		Key	  key;
		Value value;
		for (uint32_t i = 0; i < size; ++i)
		{
			stream >> key >> value;
			map[key] = value;
		}
	}

	template <typename Stream, typename U> void deserialize_vector(Stream& stream, vector<U>& vec)
	{
		uint32 sz = 0;
		stream >> sz;
		vec.resize(sz);
		if constexpr (std::is_pointer<U>::value)
		{
			for (uint32 i = 0; i < sz; i++)
			{
				using UnderlyingType = typename std::remove_pointer<U>::type;
				vec[i]				 = new UnderlyingType();
				vec[i]->deserialize(stream);
			}
		}
		else
		{
			for (uint32 i = 0; i < sz; i++)
				stream >> vec[i];
		}
	}

	class istream
	{
	public:
		void create(uint8* data, size_t size);
		void destroy();
		void read_from_ifstream(std::ifstream& stream);
		void read_to_raw_endian_safe(void* ptr, size_t size);
		void read_to_raw(uint8* ptr, size_t size);

		template <typename T> void read(T& t)
		{
			SFG_MEMCPY(reinterpret_cast<uint8*>(&t), &_data[_index], sizeof(T));
			_index += sizeof(T);
		}

		inline void skip_by(size_t size)
		{
			_index += size;
		}

		inline void seek(size_t ind)
		{
			_index = ind;
		}

		inline size_t get_size() const
		{
			return _size;
		}

		inline bool empty() const
		{
			return _size == 0;
		}

		inline uint8* get_raw() const
		{
			return _data;
		}

		inline uint8* get_data_current()
		{
			return &_data[_index];
		}

		inline void shrink(size_t size)
		{
			_size = size;
		}

	private:
		uint8* _data  = nullptr;
		size_t _index = 0;
		size_t _size  = 0;
	};

	template <typename T> istream& operator>>(istream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			stream.read(val);
			if (endianness::should_swap())
			{
				endianness::swap_endian(val);
			}
		}
		else if constexpr (std::is_same_v<T, string> || std::is_same_v<T, std::string>)
		{
			uint32 sz = 0;
			stream >> sz;
			void* d = SFG_MALLOC(sz);
			stream.read_to_raw_endian_safe(d, static_cast<size_t>(sz));
			string s((char*)d, sz);
			val = s;
			SFG_FREE(d);
		}
		else if constexpr (std::is_enum_v<T>)
		{
			uint8 u8 = 0;
			stream >> u8;
			val = static_cast<T>(u8);
		}
		else if constexpr (is_vector_v<T>)
		{
			deserialize_vector(stream, val);
		}
		else if constexpr (is_hashmap_v<T>)
		{
			using KeyType	= typename T::key_type;
			using ValueType = typename T::mapped_type;
			deserialize_hash_map(stream, val);
		}
		else if constexpr (std::is_class_v<T>)
		{
			// Handle custom classes or structs
			val.deserialize(stream);
		}
		else
		{
			SFG_ASSERT(false, "");
		}

		return stream;
	}

}

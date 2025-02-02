/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "SFG/Type/SizeDefinitions.hpp"
#include "SFG/Serialization/Endianness.hpp"
#include "SFG/Memory/Memory.hpp"
#include <fstream>

namespace SFG
{
	/*
	template <typename T> struct is_hashmap : std::false_type
	{
	};

	template <typename U, typename V> struct is_hashmap<HashMap<U, V>> : std::true_type
	{
	};

	template <typename U, typename V> struct is_hashmap<const HashMap<U, V>> : std::true_type
	{
	};

	template <typename T> inline constexpr bool is_hashmap_v = is_hashmap<T>::value;

	template <typename Stream, typename Key, typename Value> void SerializeHashMap(Stream& stream, const HashMap<Key, Value>& map)
	{
		stream << static_cast<uint32_t_t>(map.size());
		for (const auto& [key, value] : map)
		{
			stream << key << value;
		}
	}

	template <typename Stream, typename Key, typename Value> void DeserializeHashMap(Stream& stream, HashMap<Key, Value>& map)
	{
		uint32_t_t size = 0;
		stream >> size;
		Key	  key;
		Value value;
		for (uint32_t_t i = 0; i < size; ++i)
		{
			stream >> key >> value;
			map[key] = value;
		}
	}

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

	template <typename Stream, typename U> void DeserializeVector(Stream& stream, Vector<U>& vec)
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
				vec[i]->LoadFromStream(stream);
			}
		}
		else
		{
			for (uint32 i = 0; i < sz; i++)
				stream >> vec[i];
		}
	}

	template <typename Stream, typename U> void SerializeVector(Stream& stream, Vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->SaveToStream(stream);
		}
		else
		{
			for (auto& item : vec)
				stream << item;
		}
	}

	template <typename Stream, typename U> void SerializeVector(Stream& stream, const Vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->SaveToStream(stream);
		}
		else
		{
			for (const auto& item : vec)
				stream << item;
		}
	}
	*/

	class OStream
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		void Create(size_t size);

		/// <summary>
		///
		/// </summary>
		void Destroy();

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void WriteToOFStream(std::ofstream& stream);

		/// <summary>
		///
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void WriteRawEndianSafe(const uint8* ptr, size_t size);

		/// <summary>
		///
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void WriteRaw(const uint8* ptr, size_t size);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="t"></param>
		template <typename T> void Write(T& t)
		{
			if (m_data == nullptr)
				Create(sizeof(T));

			uint8* ptr	= (uint8*)&t;
			size_t size = sizeof(T);

			CheckGrow(size);
			MEMCPY(&m_data[m_currentSize], ptr, size);
			m_currentSize += size;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline size_t GetCurrentSize() const
		{
			return m_currentSize;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint8* GetDataRaw() const
		{
			return m_data;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		inline void Shrink(size_t size)
		{
			m_currentSize = size;
		}

	private:
		void CheckGrow(size_t sz);

	private:
		uint8* m_data		 = nullptr;
		size_t m_currentSize = 0;
		size_t m_totalSize	 = 0;
	};

	template <typename T> OStream& operator<<(OStream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			auto copy = const_cast<typename std::remove_const<T>::type&>(val);
			if (Endianness::ShouldSwap())
				Endianness::SwapEndian(copy);
			stream.Write<T>(copy);
		}
		// else if constexpr (std::is_same_v<T, String> || std::is_same_v<T, const String>)
		//{
		//	const uint32 sz = static_cast<uint32>(val.size());
		//	stream << sz;
		//	stream.WriteRawEndianSafe((uint8*)val.data(), val.size());
		// }
		else if constexpr (std::is_enum_v<T>)
		{
			const uint8 u8 = static_cast<uint8>(val);
			stream << u8;
		}
		//	else if constexpr (is_vector_v<T>)
		//	{
		//		SerializeVector(stream, val);
		//	}
		//	else if constexpr (is_hashmap_v<T>)
		//	{
		//		using KeyType	= typename T::key_type;
		//		using ValueType = typename T::mapped_type;
		//		SerializeHashMap(stream, val);
		//	}
		//	else if constexpr (std::is_class_v<T>)
		//	{
		//		// Handle custom classes or structs
		//		val.SaveToStream(stream);
		//	}
		//	else
		//	{
		//		SFG_ASSERT(false, "");
		//	}

		return stream;
	}

} // namespace SFG

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

#include "SFG/Serialization/Endianness.hpp"
#include "SFG/Memory/Memory.hpp"
#include <fstream>

namespace SFG
{
	class IStream
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="data"></param>
		/// <param name="size"></param>
		void Create(uint8_t* data, size_t size);

		/// <summary>
		///
		/// </summary>
		void Destroy();

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void ReadFromIFStream(std::ifstream& stream);

		/// <summary>
		///
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void ReadToRawEndianSafe(void* ptr, size_t size);

		/// <summary>
		///
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void ReadToRaw(uint8_t* ptr, size_t size);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="t"></param>
		template <typename T> void Read(T& t)
		{
			MEMCPY(reinterpret_cast<uint8_t*>(&t), &m_data[m_index], sizeof(T));
			m_index += sizeof(T);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		inline void SkipBy(size_t size)
		{
			m_index += size;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="ind"></param>
		inline void Seek(size_t ind)
		{
			m_index = ind;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline bool IsCompleted()
		{
			return m_index >= m_size;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline size_t GetSize() const
		{
			return m_size;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline bool Empty() const
		{
			return m_size == 0;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint8_t* GetDataRaw() const
		{
			return m_data;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint8_t* GetDataCurrent()
		{
			return &m_data[m_index];
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="size"></param>
		inline void Shrink(size_t size)
		{
			m_size = size;
		}

	private:
		uint8_t* m_data	 = nullptr;
		size_t	 m_index = 0;
		size_t	 m_size	 = 0;
	};

	template <typename T> IStream& operator>>(IStream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			stream.Read(val);
			if (Endianness::ShouldSwap())
			{
				Endianness::SwapEndian(val);
			}
		}
		// else if constexpr (std::is_same_v<T, String> || std::is_same_v<T, std::string>)
		// {
		// 	uint32_t sz = 0;
		// 	stream >> sz;
		// 	void* d = MALLOC(sz);
		// 	stream.ReadToRawEndianSafe(d, static_cast<size_t>(sz));
		// 	String s((char*)d, sz);
		// 	val = s;
		// 	FREE(d);
		// }
		else if constexpr (std::is_enum_v<T>)
		{
			uint8_t u8 = 0;
			stream >> u8;
			val = static_cast<T>(u8);
		}
		// else if constexpr (is_vector_v<T>)
		// {
		// 	DeserializeVector(stream, val);
		// }
		// else if constexpr (is_hashmap_v<T>)
		// {
		// 	using KeyType	= typename T::key_type;
		// 	using ValueType = typename T::mapped_type;
		// 	DeserializeHashMap(stream, val);
		// }
		// else if constexpr (std::is_class_v<T>)
		// {
		// 	// Handle custom classes or structs
		// 	val.LoadFromStream(stream);
		// }
		// else
		// {
		// 	SFG_ASSERT(false, "");
		// }

		return stream;
	}

} // namespace SFG

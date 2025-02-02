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

#include "SFG/Data/Span.hpp"

namespace SFG
{
	class OStream;
	class IStream;

	class RawStream
	{
	public:
		RawStream() : m_data({}){};
		RawStream(const RawStream& other) = delete;
		~RawStream()					  = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void Create(OStream& stream);

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
		void SaveToStream(OStream& stream) const;

		/// <summary>
		///
		/// </summary>
		/// <param name="stream"></param>
		void LoadFromStream(IStream& stream);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline Span<uint8_t> GetSpan()
		{
			return m_data;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint8_t* GetRaw() const
		{
			return m_data.data();
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline size_t GetSize() const
		{
			return m_data.size();
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		bool IsEmpty() const
		{
			return m_data.empty();
		}

	private:
		Span<uint8_t> m_data;
	};

} // namespace SFG

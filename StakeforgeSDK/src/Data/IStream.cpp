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

#include "SFG/Data/IStream.hpp"
#include "SFG/Data/Vector.hpp"

namespace SFG
{
	void IStream::Create(uint8* data, size_t size)
	{
		Destroy();
		const size_t unaligned = size;
		m_data				   = new uint8[size];

		if (data != nullptr)
			SFG_MEMCPY(m_data, data, unaligned);

		m_index = 0;
		m_size	= size;
	}

	void IStream::Destroy()
	{
		if (m_data == nullptr)
			return;

		delete[] m_data;
		m_index = 0;
		m_size	= 0;
		m_data	= nullptr;
	}

	void IStream::ReadFromIFStream(std::ifstream& stream)
	{
		stream.read((char*)m_data, m_size);
	}

	void IStream::ReadToRawEndianSafe(void* ptr, size_t size)
	{
		if (Endianness::ShouldSwap())
		{
			uint8*		  data = &m_data[m_index];
			Vector<uint8> v;
			v.insert(v.end(), data, data + size);

			Vector<uint8> v2;
			v2.resize(v.size());
			std::reverse_copy(v.begin(), v.end(), v2.begin());

			SFG_MEMCPY(ptr, v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			SFG_MEMCPY(ptr, &m_data[m_index], size);

		m_index += size;
	}

	void IStream::ReadToRaw(uint8* ptr, size_t size)
	{
		SFG_MEMCPY(ptr, &m_data[m_index], size);
		m_index += size;
	}

} // namespace SFG

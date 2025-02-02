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

#include "SFG/Data/OStream.hpp"
#include "SFG/Data/Vector.hpp"

namespace SFG
{
	void OStream::Create(size_t size)
	{
		m_data		  = new uint8_t[size];
		m_totalSize	  = size;
		m_currentSize = 0;
	}

	void OStream::Destroy()
	{
		delete[] m_data;

		m_currentSize = 0;
		m_totalSize	  = 0;
		m_data		  = nullptr;
	}

	void OStream::WriteRawEndianSafe(const uint8_t* ptr, size_t size)
	{
		if (m_data == nullptr)
			Create(size);

		CheckGrow(size);

		if (Endianness::ShouldSwap())
		{
			Vector<uint8_t> v;
			v.insert(v.end(), ptr, (ptr) + size);

			Vector<uint8_t> v2;
			v2.resize(v.size());
			std::reverse_copy(v.begin(), v.end(), v2.begin());
			MEMCPY(&m_data[m_currentSize], v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			MEMCPY(&m_data[m_currentSize], ptr, size);

		m_currentSize += size;
	}

	void OStream::WriteRaw(const uint8_t* ptr, size_t size)
	{
		if (m_data == nullptr)
			Create(size);

		CheckGrow(size);
		MEMCPY(&m_data[m_currentSize], ptr, size);
		m_currentSize += size;
	}

	void OStream::CheckGrow(size_t sz)
	{
		if (m_currentSize + sz > m_totalSize)
		{
			m_totalSize		 = static_cast<size_t>((static_cast<float>(m_currentSize + sz) * 2.0f));
			uint8_t* newData = new uint8_t[m_totalSize];
			MEMCPY(newData, m_data, m_currentSize);
			delete[] m_data;
			m_data = newData;
		}
	}
	void OStream::WriteToOFStream(std::ofstream& stream)
	{
		stream.write((char*)m_data, m_currentSize);
	}

} // namespace SFG

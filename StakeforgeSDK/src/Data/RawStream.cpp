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

#include "SFG/Data/RawStream.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Memory/Memory.hpp"

namespace SFG
{
	void RawStream::Create(uint8* data, size_t size)
	{
		Destroy();
		m_data = {new uint8[size], size};
		MEMCPY(m_data.data(), data, size);
	}

	void RawStream::Create(OStream& stream)
	{
		Destroy();
		m_data = {new uint8[stream.GetCurrentSize()], stream.GetCurrentSize()};
		MEMCPY(m_data.data(), stream.GetDataRaw(), stream.GetCurrentSize());
	}

	void RawStream::Destroy()
	{
		if (IsEmpty())
			return;
		delete[] m_data.data();
		m_data = {};
	}

	void RawStream::SaveToStream(OStream& stream) const
	{
		const uint32 sz = static_cast<uint32>(m_data.size());
		stream.Write(sz);
		if (sz != 0)
			stream.WriteRaw(m_data.data(), m_data.size());
	}

	void RawStream::LoadFromStream(IStream& stream)
	{
		uint32 size = 0;
		stream.Read(size);
		if (size != 0)
		{
			const size_t sz = static_cast<size_t>(size);
			Destroy();
			m_data = {new uint8[sz], sz};
			stream.ReadToRaw(m_data.data(), m_data.size());
		}
	}

} // namespace SFG

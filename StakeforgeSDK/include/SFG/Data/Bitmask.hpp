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

namespace SFG
{
	template <typename T> class Bitmask
	{
	public:
		Bitmask()  = default;
		~Bitmask() = default;

		Bitmask(T m) : m_mask(m){};

		inline bool IsSet(T m) const
		{
			return (m_mask & m) != 0;
		}

		inline bool IsAllSet(T bits) const
		{
			return (m_mask & bits) == bits;
		}

		inline void Set(T m)
		{
			m_mask |= m;
		}

		inline void Set(T m, bool isSet)
		{
			if (isSet)
				m_mask |= m;
			else
				m_mask &= ~m;
		}

		inline void Remove(T m)
		{
			m_mask &= ~m;
		}

		inline T GetValue() const
		{
			return m_mask;
		}

	private:
		T m_mask = 0;
	};

	typedef Bitmask<uint8_t>  Bitmask8;
	typedef Bitmask<uint16_t> Bitmask16;
	typedef Bitmask<uint32_t> Bitmask32;
} // namespace SFG

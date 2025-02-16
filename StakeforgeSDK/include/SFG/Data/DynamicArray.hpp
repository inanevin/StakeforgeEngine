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

#include <atomic>

namespace SFG
{
	template <typename T> class DynamicArray
	{
	public:
		DynamicArray() = default;

		explicit DynamicArray(uint32 size)
		{
			if (m_ptr && size == m_size)
			{
				for (uint32 i = 0; i < size; i++)
					m_ptr[i] = T();
				return;
			}

			if (m_ptr)
				delete[] m_ptr;

			m_size = size;
			m_ptr  = new T[size]();
		}

		~DynamicArray()
		{
			m_size = 0;
			delete[] m_ptr;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="idx"></param>
		/// <returns></returns>
		T& operator[](uint32 idx)
		{
			return m_ptr[idx];
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		T* begin()
		{
			return m_ptr;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		T* end()
		{
			return m_ptr + m_size;
		}
		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		const T* begin() const
		{
			return m_ptr;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		const T* end() const
		{
			return m_ptr + m_size;
		}

	private:
		T*	   m_ptr  = nullptr;
		uint32 m_size = 0;
	};
} // namespace SFG

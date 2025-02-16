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
#include "SFG/Data/Vector.hpp"

namespace SFG
{
	namespace UtilVector
	{

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="Predicate"></typeparam>
		/// <param name="vec"></param>
		/// <param name="outValue"></param>
		/// <param name="pred"></param>
		/// <returns></returns>
		template <typename T, class Predicate> inline bool Find(const Vector<T>& vec, T& outValue, Predicate pred)
		{
			auto it = std::find_if(vec.begin(), vec.end(), pred);
			if (it != vec.end())
			{
				outValue = *it;
				return true;
			}

			return false;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="value"></param>
		template <typename T> inline Vector<T>::iterator Remove(Vector<T>& vec, T& value)
		{
			return vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		template <typename T> inline int32 IndexOf(const Vector<T>& vec, const T& value)
		{
			const int32 sz = static_cast<int32>(vec.size());

			for (int32 i = 0; i < sz; ++i)
			{
				if (vec[i] == value)
					return i;
			}

			return -1;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		template <typename T> inline int32 FindNextIndexIfItemIsRemoved(const Vector<T>& vec, const T& value)
		{
			const int32 currentIndex = IndexOf(vec, value);

			if (currentIndex == 0)
			{
				if (vec.size() == 1)
					return -1;

				return 0;
			}

			return currentIndex - 1;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="src"></param>
		/// <param name="target"></param>
		template <typename T> inline void PlaceAfter(Vector<T>& vec, T& src, T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });
			vec.erase(itSrc);
			vec.insert(itTarget + 1, *itSrc);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="src"></param>
		/// <param name="target"></param>
		template <typename T> inline void PlaceBefore(Vector<T>& vec, const T& src, const T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });

			vec.insert(itTarget, *itSrc);
			if (itSrc < itTarget)
				vec.erase(itSrc);
			else
				vec.erase(itSrc + 1);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="vec"></param>
		/// <param name="data"></param>
		/// <returns></returns>
		template <typename T> inline bool Contains(const Vector<T>& vec, const T& data)
		{
			for (const auto& v : vec)
			{
				if (v == data)
					return true;
			}

			return false;
		}

	} // namespace UtilVector

} // namespace SFG

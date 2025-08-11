// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"

namespace SFG
{
	namespace vector_util
	{

		template <typename T, class Predicate> inline bool find(const vector<T>& vec, T& outValue, Predicate pred)
		{
			auto it = std::find_if(vec.begin(), vec.end(), pred);
			if (it != vec.end())
			{
				outValue = *it;
				return true;
			}

			return false;
		}

		template <typename T, class Predicate> inline vector<T>::const_iterator find_if(const vector<T>& vec, Predicate pred)
		{
			return std::find_if(vec.cbegin(), vec.cend(), pred);
		}

		template <typename T, class Predicate> inline vector<T>::iterator find_if(vector<T>& vec, Predicate pred)
		{
			return std::find_if(vec.begin(), vec.end(), pred);
		}

		template <typename T, class Predicate> inline uint32 erase_if(vector<T>& vec, Predicate pred)
		{
			return static_cast<uint32>(std::erase_if(vec, pred));
		}

		template <typename T> inline vector<T>::iterator remove(vector<T>& vec, T& value)
		{
			return vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
		}

		template <typename T> inline int32 index_of(const vector<T>& vec, const T& value)
		{
			const int32 sz = static_cast<int32>(vec.size());

			for (int32 i = 0; i < sz; ++i)
			{
				if (vec[i] == value)
					return i;
			}

			return -1;
		}

		template <typename T> inline int32 find_next_index_if_removed(const vector<T>& vec, const T& value)
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

		template <typename T> inline void place_after(vector<T>& vec, T& src, T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });
			vec.erase(itSrc);
			vec.insert(itTarget + 1, *itSrc);
		}

		template <typename T> inline void place_before(vector<T>& vec, const T& src, const T& target)
		{
			auto itSrc	  = std::find_if(vec.begin(), vec.end(), [src](const T& child) { return child == src; });
			auto itTarget = std::find_if(vec.begin(), vec.end(), [target](const T& child) { return child == target; });

			vec.insert(itTarget, *itSrc);
			if (itSrc < itTarget)
				vec.erase(itSrc);
			else
				vec.erase(itSrc + 1);
		}

		template <typename T> inline bool contains(const vector<T>& vec, const T& data)
		{
			for (const auto& v : vec)
			{
				if (v == data)
					return true;
			}

			return false;
		}

	} // namespace Utilvector
}
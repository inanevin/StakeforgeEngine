#if !defined(phmap_dump_h_guard_)
#define phmap_dump_h_guard_

// ---------------------------------------------------------------------------
// Copyright (c) 2019, Gregory Popovitch - greg7mdp@gmail.com
//
//       providing dump/load/mmap_load
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include "phmap.h"
namespace phmap
{

	namespace type_traits_internal
	{

#if defined(__GLIBCXX__) && __GLIBCXX__ < 20150801
		template <typename T> struct IsTriviallyCopyable : public std::integral_constant<bool, __has_trivial_copy(T)>
		{
		};
#else
		template <typename T> struct IsTriviallyCopyable : public std::is_trivially_copyable<T>
		{
		};
#endif

		template <class T1, class T2> struct IsTriviallyCopyable<std::pair<T1, T2>>
		{
			static constexpr bool value = IsTriviallyCopyable<T1>::value && IsTriviallyCopyable<T2>::value;
		};
	}

	namespace priv
	{

#if !defined(PHMAP_NON_DETERMINISTIC) && !defined(PHMAP_DISABLE_DUMP)

		// ------------------------------------------------------------------------
		// dump/load for raw_hash_set
		// ------------------------------------------------------------------------
		template <class Policy, class Hash, class Eq, class Alloc> template <typename OutputArchive> bool raw_hash_set<Policy, Hash, Eq, Alloc>::phmap_dump(OutputArchive& ar) const
		{
			static_assert(type_traits_internal::IsTriviallyCopyable<value_type>::value, "value_type should be trivially copyable");

			ar.saveBinary(&size_, sizeof(size_t));
			if (size_ == 0) return true;
			ar.saveBinary(&capacity_, sizeof(size_t));
			ar.saveBinary(ctrl_, sizeof(ctrl_t) * (capacity_ + Group::kWidth + 1));
			ar.saveBinary(slots_, sizeof(slot_type) * capacity_);
			return true;
		}

		template <class Policy, class Hash, class Eq, class Alloc> template <typename InputArchive> bool raw_hash_set<Policy, Hash, Eq, Alloc>::phmap_load(InputArchive& ar)
		{
			static_assert(type_traits_internal::IsTriviallyCopyable<value_type>::value, "value_type should be trivially copyable");
			raw_hash_set<Policy, Hash, Eq, Alloc>().swap(*this); // clear any existing content
			ar.loadBinary(&size_, sizeof(size_t));
			if (size_ == 0) return true;
			ar.loadBinary(&capacity_, sizeof(size_t));

			// allocate memory for ctrl_ and slots_
			initialize_slots(capacity_);
			ar.loadBinary(ctrl_, sizeof(ctrl_t) * (capacity_ + Group::kWidth + 1));
			ar.loadBinary(slots_, sizeof(slot_type) * capacity_);
			return true;
		}

		// ------------------------------------------------------------------------
		// dump/load for parallel_hash_set
		// ------------------------------------------------------------------------
		template <size_t N, template <class, class, class, class> class RefSet, class Mtx_, class Policy, class Hash, class Eq, class Alloc>
		template <typename OutputArchive>
		bool parallel_hash_set<N, RefSet, Mtx_, Policy, Hash, Eq, Alloc>::phmap_dump(OutputArchive& ar) const
		{
			static_assert(type_traits_internal::IsTriviallyCopyable<value_type>::value, "value_type should be trivially copyable");

			size_t submap_count = subcnt();
			ar.saveBinary(&submap_count, sizeof(size_t));
			for (size_t i = 0; i < sets_.size(); ++i)
			{
				auto&						  inner = sets_[i];
				typename Lockable::UniqueLock m(const_cast<Inner&>(inner));
				if (!inner.set_.phmap_dump(ar))
				{
					std::cerr << "Failed to dump submap " << i << std::endl;
					return false;
				}
			}
			return true;
		}

		template <size_t N, template <class, class, class, class> class RefSet, class Mtx_, class Policy, class Hash, class Eq, class Alloc>
		template <typename InputArchive>
		bool parallel_hash_set<N, RefSet, Mtx_, Policy, Hash, Eq, Alloc>::phmap_load(InputArchive& ar)
		{
			static_assert(type_traits_internal::IsTriviallyCopyable<value_type>::value, "value_type should be trivially copyable");

			size_t submap_count = 0;
			ar.loadBinary(&submap_count, sizeof(size_t));
			if (submap_count != subcnt())
			{
				std::cerr << "submap count(" << submap_count << ") != N(" << N << ")" << std::endl;
				return false;
			}

			for (size_t i = 0; i < submap_count; ++i)
			{
				auto&						  inner = sets_[i];
				typename Lockable::UniqueLock m(const_cast<Inner&>(inner));
				if (!inner.set_.phmap_load(ar))
				{
					std::cerr << "Failed to load submap " << i << std::endl;
					return false;
				}
			}
			return true;
		}

#endif // !defined(PHMAP_NON_DETERMINISTIC) && !defined(PHMAP_DISABLE_DUMP)

	} // namespace priv

	// ------------------------------------------------------------------------
	// BinaryArchive
	//       File is closed when archive object is destroyed
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	class BinaryOutputArchive
	{
	public:
		BinaryOutputArchive(const char* file_path)
		{
			ofs_.open(file_path, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		}

		bool saveBinary(const void* p, size_t sz)
		{
			ofs_.write(reinterpret_cast<const char*>(p), sz);
			return true;
		}

	private:
		std::ofstream ofs_;
	};

	class BinaryInputArchive
	{
	public:
		BinaryInputArchive(const char* file_path)
		{
			ifs_.open(file_path, std::ofstream::in | std::ofstream::binary);
		}

		bool loadBinary(void* p, size_t sz)
		{
			ifs_.read(reinterpret_cast<char*>(p), sz);
			return true;
		}

	private:
		std::ifstream ifs_;
	};

} // namespace phmap

#ifdef CEREAL_SIZE_TYPE

template <class T> using PhmapTrivCopyable = typename phmap::type_traits_internal::IsTriviallyCopyable<T>;

namespace cereal
{
	// Overload Cereal serialization code for phmap::flat_hash_map
	// -----------------------------------------------------------
	template <class K, class V, class Hash, class Eq, class A> void save(typename std::enable_if<PhmapTrivCopyable<K>::value && PhmapTrivCopyable<V>::value, typename cereal::BinaryOutputArchive>::type& ar, phmap::flat_hash_map<K, V, Hash, Eq, A> const& hmap)
	{
		hmap.phmap_dump(ar);
	}

	template <class K, class V, class Hash, class Eq, class A> void load(typename std::enable_if<PhmapTrivCopyable<K>::value && PhmapTrivCopyable<V>::value, typename cereal::BinaryInputArchive>::type& ar, phmap::flat_hash_map<K, V, Hash, Eq, A>& hmap)
	{
		hmap.phmap_load(ar);
	}

	// Overload Cereal serialization code for phmap::parallel_flat_hash_map
	// --------------------------------------------------------------------
	template <class K, class V, class Hash, class Eq, class A, size_t N, class Mtx_>
	void save(typename std::enable_if<PhmapTrivCopyable<K>::value && PhmapTrivCopyable<V>::value, typename cereal::BinaryOutputArchive>::type& ar, phmap::parallel_flat_hash_map<K, V, Hash, Eq, A, N, Mtx_> const& hmap)
	{
		hmap.phmap_dump(ar);
	}

	template <class K, class V, class Hash, class Eq, class A, size_t N, class Mtx_>
	void load(typename std::enable_if<PhmapTrivCopyable<K>::value && PhmapTrivCopyable<V>::value, typename cereal::BinaryInputArchive>::type& ar, phmap::parallel_flat_hash_map<K, V, Hash, Eq, A, N, Mtx_>& hmap)
	{
		hmap.phmap_load(ar);
	}

	// Overload Cereal serialization code for phmap::flat_hash_set
	// -----------------------------------------------------------
	template <class K, class Hash, class Eq, class A> void save(typename std::enable_if<PhmapTrivCopyable<K>::value, typename cereal::BinaryOutputArchive>::type& ar, phmap::flat_hash_set<K, Hash, Eq, A> const& hset)
	{
		hset.phmap_dump(ar);
	}

	template <class K, class Hash, class Eq, class A> void load(typename std::enable_if<PhmapTrivCopyable<K>::value, typename cereal::BinaryInputArchive>::type& ar, phmap::flat_hash_set<K, Hash, Eq, A>& hset)
	{
		hset.phmap_load(ar);
	}

	// Overload Cereal serialization code for phmap::parallel_flat_hash_set
	// --------------------------------------------------------------------
	template <class K, class Hash, class Eq, class A, size_t N, class Mtx_> void save(typename std::enable_if<PhmapTrivCopyable<K>::value, typename cereal::BinaryOutputArchive>::type& ar, phmap::parallel_flat_hash_set<K, Hash, Eq, A, N, Mtx_> const& hset)
	{
		hset.phmap_dump(ar);
	}

	template <class K, class Hash, class Eq, class A, size_t N, class Mtx_> void load(typename std::enable_if<PhmapTrivCopyable<K>::value, typename cereal::BinaryInputArchive>::type& ar, phmap::parallel_flat_hash_set<K, Hash, Eq, A, N, Mtx_>& hset)
	{
		hset.phmap_load(ar);
	}
}

#endif

#endif // phmap_dump_h_guard_

// Copyright (c) 2025 Inan Evin

#include "ostream.hpp"
#include "data/vector.hpp"

namespace Game
{
	void ostream::create(size_t size)
	{
		_data		  = new uint8[size];
		_total_size	  = size;
		_current_size = 0;
	}

	void ostream::destroy()
	{
		delete[] _data;

		_current_size = 0;
		_total_size	  = 0;
		_data		  = nullptr;
	}

	void ostream::write_raw_endian_safe(const uint8* ptr, size_t size)
	{
		if (_data == nullptr)
			create(size);

		check_grow(size);

		if (endianness::should_swap())
		{
			vector<uint8> v;
			v.insert(v.end(), ptr, (ptr) + size);

			vector<uint8> v2;
			v2.resize(v.size());
			std::reverse_copy(v.begin(), v.end(), v2.begin());
			SFG_MEMCPY(&_data[_current_size], v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			SFG_MEMCPY(&_data[_current_size], ptr, size);

		_current_size += size;
	}

	void ostream::write_raw(const uint8* ptr, size_t size)
	{
		if (_data == nullptr)
			create(size);

		check_grow(size);
		SFG_MEMCPY(&_data[_current_size], ptr, size);
		_current_size += size;
	}

	void ostream::check_grow(size_t sz)
	{
		if (_current_size + sz > _total_size)
		{
			_total_size	   = static_cast<size_t>((static_cast<float>(_current_size + sz) * 2.0f));
			uint8* newData = new uint8[_total_size];
			SFG_MEMCPY(newData, _data, _current_size);
			delete[] _data;
			_data = newData;
		}
	}
	void ostream::write_to_ofstream(std::ofstream& stream)
	{
		stream.write((char*)_data, _current_size);
	}

}

// Copyright (c) 2025 Inan Evin

#include "istream.hpp"
#include "vector.hpp"

namespace Game
{
	void istream::create(uint8* data, size_t size)
	{
		destroy();
		const size_t unaligned = size;
		_data				   = new uint8[size];

		if (data != nullptr)
			SFG_MEMCPY(_data, data, unaligned);

		_index = 0;
		_size  = size;
	}

	void istream::destroy()
	{
		if (_data == nullptr)
			return;

		delete[] _data;
		_index = 0;
		_size  = 0;
		_data  = nullptr;
	}

	void istream::read_from_ifstream(std::ifstream& stream)
	{
		stream.read((char*)_data, _size);
	}

	void istream::read_to_raw_endian_safe(void* ptr, size_t size)
	{
		if (endianness::should_swap())
		{
			uint8*		  data = &_data[_index];
			vector<uint8> v;
			v.insert(v.end(), data, data + size);

			vector<uint8> v2;
			v2.resize(v.size());
			std::reverse_copy(v.begin(), v.end(), v2.begin());

			SFG_MEMCPY(ptr, v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			SFG_MEMCPY(ptr, &_data[_index], size);

		_index += size;
	}

	void istream::read_to_raw(uint8* ptr, size_t size)
	{
		SFG_MEMCPY(ptr, &_data[_index], size);
		_index += size;
	}

}

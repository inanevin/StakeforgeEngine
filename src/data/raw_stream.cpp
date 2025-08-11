// Copyright (c) 2025 Inan Evin

#include "raw_stream.hpp"
#include "ostream.hpp"
#include "istream.hpp"
#include "memory/memory.hpp"

namespace SFG
{
	void raw_stream::create(uint8* data, size_t size)
	{
		destroy();
		_data = {new uint8[size], size};
		SFG_MEMCPY(_data.data, data, size);
	}

	void raw_stream::create(ostream& stream)
	{
		destroy();
		_data = {new uint8[stream.get_size()], stream.get_size()};
		SFG_MEMCPY(_data.data, stream.get_raw(), stream.get_size());
	}

	void raw_stream::destroy()
	{
		if (is_empty())
			return;
		delete[] _data.data;
		_data = {};
	}

	void raw_stream::serialize(ostream& stream) const
	{
		const uint32 sz = static_cast<uint32>(_data.size);
		stream.write(sz);
		if (sz != 0)
			stream.write_raw(_data.data, _data.size);
	}

	void raw_stream::deserialize(istream& stream)
	{
		uint32 size = 0;
		stream.read(size);
		if (size != 0)
		{
			const size_t sz = static_cast<size_t>(size);
			destroy();
			_data = {new uint8[sz], sz};
			stream.read_to_raw(_data.data, _data.size);
		}
	}

}

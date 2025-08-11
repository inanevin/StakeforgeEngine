// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "span.hpp"

namespace SFG
{
	class ostream;
	class istream;

	class raw_stream
	{
	public:
		raw_stream() : _data({}){};
		~raw_stream() = default;

		raw_stream(const raw_stream& other)			   = delete;
		raw_stream& operator=(const raw_stream& other) = delete;

		void create(ostream& stream);
		void create(uint8* data, size_t size);
		void destroy();
		void serialize(ostream& stream) const;
		void deserialize(istream& stream);

		inline span<uint8> get_span()
		{
			return _data;
		}

		inline uint8* get_raw() const
		{
			return _data.data;
		}

		inline size_t get_size() const
		{
			return _data.size;
		}

		bool is_empty() const
		{
			return _data.size == 0;
		}

	private:
		span<uint8> _data;
	};

}

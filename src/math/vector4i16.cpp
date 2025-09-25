// Copyright (c) 2025 Inan Evin

#include "vector4i16.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	vector4i16 vector4i16::zero = vector4i16(0, 0, 0, 0);
	vector4i16 vector4i16::one	= vector4i16(1, 1, 1, 1);

	void vector4i16::serialize(ostream& stream) const
	{
		stream << x << y << z << w;
	}
	void vector4i16::deserialize(istream& stream)
	{
		stream >> x >> y >> z >> w;
	}

}
// Copyright (c) 2025 Inan Evin

#include "vector2ui16.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	vector2ui16 vector2ui16::zero = vector2ui16(0, 0);
	vector2ui16 vector2ui16::one  = vector2ui16(1, 1);

	void vector2ui16::serialize(ostream& out) const
	{
		out << x << y;
	}

	void vector2ui16::deserialize(istream& in)
	{
		in >> x >> y;
	}
}
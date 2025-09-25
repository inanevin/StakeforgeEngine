// Copyright (c) 2025 Inan Evin

#include "skin_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	void skin_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << sid;
		stream << joints;
		stream << root_joint;
	}
	void skin_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> sid;
		stream >> joints;
		stream >> root_joint;
	}
}

// Copyright (c) 2025 Inan Evin

#include "animation_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{

	void animation_channel_v3_raw::serialize(ostream& stream) const
	{
		stream << interpolation;
		stream << keyframes;
		stream << keyframes_spline;
		stream << node_index;
	}

	void animation_channel_v3_raw::deserialize(istream& stream)
	{
		stream >> interpolation;
		stream >> keyframes;
		stream >> keyframes_spline;
		stream >> node_index;
	}

	void animation_channel_q_raw::serialize(ostream& stream) const
	{
		stream << interpolation;
		stream << keyframes;
		stream << keyframes_spline;
		stream << node_index;
	}

	void animation_channel_q_raw::deserialize(istream& stream)
	{
		stream >> interpolation;
		stream >> keyframes;
		stream >> keyframes_spline;
		stream >> node_index;
	}

	void animation_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << sid;
		stream << duration;
		stream << position_channels;
		stream << rotation_channels;
		stream << scale_channels;
	}

	void animation_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> sid;
		stream >> duration;
		stream >> position_channels;
		stream >> rotation_channels;
		stream >> scale_channels;
	}

}

// Copyright (c) 2025 Inan Evin

#include "animation_common.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	void animation_keyframe_v3::serialize(ostream& stream) const
	{
		stream << time;
		stream << value;
	}
	void animation_keyframe_v3::deserialize(istream& stream)
	{
		stream >> time;
		stream >> value;
	}

	void animation_keyframe_v3_spline::serialize(ostream& stream) const
	{
		stream << time;
		stream << value;
		stream << in_tangent;
		stream << out_tangent;
	}
	void animation_keyframe_v3_spline::deserialize(istream& stream)
	{
		stream >> time;
		stream >> value;
		stream >> in_tangent;
		stream >> out_tangent;
	}

	void animation_keyframe_q::serialize(ostream& stream) const
	{
		stream << time;
		stream << value;
	}
	void animation_keyframe_q::deserialize(istream& stream)
	{
		stream >> time;
		stream >> value;
	}

	void animation_keyframe_q_spline::serialize(ostream& stream) const
	{
		stream << time;
		stream << value;
		stream << in_tangent;
		stream << out_tangent;
	}
	void animation_keyframe_q_spline::deserialize(istream& stream)
	{
		stream >> time;
		stream >> value;
		stream >> in_tangent;
		stream >> out_tangent;
	}
}
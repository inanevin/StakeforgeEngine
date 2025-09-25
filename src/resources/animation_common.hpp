// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"

namespace SFG
{
	enum class animation_interpolation : uint8
	{
		linear,
		step,
		cubic_spline,
	};

	class ostream;
	class istream;

	struct animation_keyframe_v3
	{
		float	time  = 0.0f;
		vector3 value = vector3::zero;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};

	struct animation_keyframe_v3_spline
	{
		float	time		= 0.0f;
		vector3 in_tangent	= vector3::zero;
		vector3 value		= vector3::zero;
		vector3 out_tangent = vector3::zero;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};

	struct animation_keyframe_q
	{
		float time	= 0.0f;
		quat  value = quat();

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};

	struct animation_keyframe_q_spline
	{
		float time		  = 0.0f;
		quat  in_tangent  = quat();
		quat  value		  = quat();
		quat  out_tangent = quat();

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};
}
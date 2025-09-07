// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	enum class animation_interpolation
	{
		linear,
		step,
		cubic_spline,
	};

	struct animation_keyframe_v3
	{
		float	time  = 0.0f;
		vector3 value = vector3::zero;
	};

	struct animation_keyframe_v3_spline
	{
		float	time		= 0.0f;
		vector3 in_tangent	= vector3::zero;
		vector3 value		= vector3::zero;
		vector3 out_tangent = vector3::zero;
	};

	struct animation_keyframe_q
	{
		float time	= 0.0f;
		quat  value = quat();
	};

	struct animation_keyframe_q_spline
	{
		float time		  = 0.0f;
		quat  in_tangent  = quat();
		quat  value		  = quat();
		quat  out_tangent = quat();
	};

	struct animation_channel_v3
	{
		animation_interpolation				 interpolation = animation_interpolation::linear;
		vector<animation_keyframe_v3>		 keyframes;
		vector<animation_keyframe_v3_spline> keyframes_spline;
		int16								 node_index = -1;

		vector3 sample(float time) const;
	};

	struct animation_channel_q
	{
		animation_interpolation				interpolation = animation_interpolation::linear;
		vector<animation_keyframe_q>		keyframes;
		vector<animation_keyframe_q_spline> keyframes_spline;
		int16								node_index = -1;

		quat sample(float time) const;
	};

	struct animation
	{
#ifdef SFG_TOOLMODE
		string name = "";
#endif
		float						 duration = 0.0f;
		vector<animation_channel_v3> position_channels;
		vector<animation_channel_v3> scale_channels;
		vector<animation_channel_q>	 rotation_channels;
	};
}
// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/string_id.hpp"
#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "world/common_world.hpp"
#include "resources/common_resources.hpp"
#include "memory/chunk_handle.hpp"

#ifdef SFG_TOOLMODE
#include "data/string.hpp"
#endif

namespace SFG
{
	class chunk_allocator32;

	enum class animation_interpolation : uint8
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

	struct animation_channel_v3_loaded
	{
		animation_interpolation				 interpolation = animation_interpolation::linear;
		vector<animation_keyframe_v3>		 keyframes;
		vector<animation_keyframe_v3_spline> keyframes_spline;
		int16								 node_index = -1;
	};

	struct animation_channel_v3
	{
		animation_interpolation interpolation = animation_interpolation::linear;
		chunk_handle32			keyframes;
		chunk_handle32			keyframes_spline;
		int16					node_index = -1;

		void create_from_loaded(const animation_channel_v3_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

		vector3 sample(float time, chunk_allocator32& alloc) const;
	};

	struct animation_channel_q_loaded
	{
		animation_interpolation				interpolation = animation_interpolation::linear;
		vector<animation_keyframe_q>		keyframes;
		vector<animation_keyframe_q_spline> keyframes_spline;
		int16								node_index = -1;
	};

	struct animation_channel_q
	{
		animation_interpolation interpolation = animation_interpolation::linear;
		chunk_handle32			keyframes;
		chunk_handle32			keyframes_spline;
		int16					node_index = -1;

		void create_from_loaded(const animation_channel_q_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

		quat sample(float time, chunk_allocator32& alloc) const;
	};

	struct animation_loaded
	{
		string								name	 = "";
		string_id							sid		 = 0;
		float								duration = 0.0f;
		vector<animation_channel_v3_loaded> position_channels;
		vector<animation_channel_q_loaded>	rotation_channels;
		vector<animation_channel_v3_loaded> scale_channels;
	};

	class animation
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_animation;

		void create_from_loaded(const animation_loaded& loaded, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);

	private:
		float		   _duration = 0.0f;
		chunk_handle32 _name;
		chunk_handle32 _position_channels;
		chunk_handle32 _scale_channels;
		chunk_handle32 _rotation_channels;
		uint16		   _position_count = 0;
		uint16		   _rotation_count = 0;
		uint16		   _scale_count	   = 0;
	};
}
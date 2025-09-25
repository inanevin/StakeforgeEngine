// Copyright (c) 2025 Inan Evin
#pragma once

#include "animation_common.hpp"
#include "memory/chunk_handle.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	class chunk_allocator32;
	struct animation_channel_v3_raw;
	struct animation_channel_q_raw;
	struct animation_raw;

	struct animation_channel_v3
	{
		animation_interpolation interpolation = animation_interpolation::linear;
		chunk_handle32			keyframes;
		chunk_handle32			keyframes_spline;
		int16					node_index = -1;

		void	create_from_raw(const animation_channel_v3_raw& raw, chunk_allocator32& alloc);
		void	destroy(chunk_allocator32& alloc);
		vector3 sample(float time, chunk_allocator32& alloc) const;
	};

	struct animation_channel_q
	{
		animation_interpolation interpolation = animation_interpolation::linear;
		chunk_handle32			keyframes;
		chunk_handle32			keyframes_spline;
		int16					node_index = -1;

		void create_from_raw(const animation_channel_q_raw& raw, chunk_allocator32& alloc);
		void destroy(chunk_allocator32& alloc);
		quat sample(float time, chunk_allocator32& alloc) const;
	};

	class animation
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_animation;

		void create_from_raw(const animation_raw& raw, chunk_allocator32& alloc);
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
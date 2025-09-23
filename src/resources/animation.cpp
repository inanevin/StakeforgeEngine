// Copyright (c) 2025 Inan Evin

#include "animation.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
	void animation_channel_v3::create_from_loaded(const animation_channel_v3_loaded& loaded, chunk_allocator32& alloc)
	{

		interpolation = loaded.interpolation;
		node_index	  = loaded.node_index;

		const uint32 kf_count		  = static_cast<uint32>(loaded.keyframes.size());
		const uint32 kf_splines_count = static_cast<uint32>(loaded.keyframes_spline.size());

		if (kf_count != 0)
		{
			keyframes					  = alloc.allocate<animation_keyframe_v3>(kf_count);
			animation_keyframe_v3* ptr_kf = alloc.get<animation_keyframe_v3>(keyframes);
			SFG_MEMCPY(ptr_kf, loaded.keyframes.data(), keyframes.size);
		}

		if (kf_splines_count != 0)
		{
			keyframes_spline					 = alloc.allocate<animation_keyframe_v3_spline>(kf_splines_count);
			animation_keyframe_v3_spline* ptr_kf = alloc.get<animation_keyframe_v3_spline>(keyframes_spline);
			SFG_MEMCPY(ptr_kf, loaded.keyframes_spline.data(), keyframes_spline.size);
		}
	}

	void animation_channel_v3::destroy(chunk_allocator32& alloc)
	{
		if (keyframes.size != 0)
			alloc.free(keyframes);

		if (keyframes_spline.size != 0)
			alloc.free(keyframes_spline);
	}

	vector3 animation_channel_v3::sample(float time, chunk_allocator32& alloc) const
	{
		if (keyframes.size == 0 && keyframes_spline.size == 0)
			return vector3::zero; // Return a default value.

		animation_keyframe_v3*		  ptr					 = alloc.get<animation_keyframe_v3>(keyframes);
		animation_keyframe_v3_spline* ptr_spline			 = alloc.get<animation_keyframe_v3_spline>(keyframes_spline);
		const uint32				  keyframes_count		 = keyframes.size == 0 ? 0 : sizeof(animation_keyframe_v3) / keyframes.size;
		const uint32				  keyframes_spline_count = keyframes.size == 0 ? 0 : sizeof(animation_keyframe_v3_spline) / keyframes_spline.size;

		if (interpolation == animation_interpolation::cubic_spline && keyframes_spline_count != 0)
		{
			const animation_keyframe_v3_spline& front = ptr_spline[0];
			const animation_keyframe_v3_spline& back  = ptr_spline[keyframes_spline_count - 1];
			if (time <= front.time)
				return front.value;
			if (time >= back.time)
				return back.value;

			size_t i = 0;
			while (i < keyframes_spline_count - 1 && time > ptr_spline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = ptr_spline[i];
			const auto& kf1 = ptr_spline[i + 1];

			float t0 = kf0.time;
			float t1 = kf1.time;

			// normalized time
			float localT = (time - t0) / (t1 - t0);

			// cubic Hermite spline interpolation.
			float t	 = localT;
			float t2 = t * t;
			float t3 = t2 * t;

			float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
			float h10 = t3 - 2.0f * t2 + t;
			float h01 = -2.0f * t3 + 3.0f * t2;
			float h11 = t3 - t2;

			return h00 * kf0.value + h10 * kf0.out_tangent * (t1 - t0) + h01 * kf1.value + h11 * kf1.in_tangent * (t1 - t0);
		}
		else if (keyframes_count != 0)
		{
			const animation_keyframe_v3& front = ptr[0];
			const animation_keyframe_v3& back  = ptr[keyframes_count - 1];

			if (time <= front.time)
				return front.value;
			if (time >= back.time)
				return back.value;

			size_t i = 0;
			while (i < keyframes_count - 1 && time > ptr[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = ptr[i];
			const auto& kf1 = ptr[i + 1];

			float t0 = kf0.time;
			float t1 = kf1.time;

			float localT = (time - t0) / (t1 - t0);

			switch (interpolation)
			{
			case animation_interpolation::linear:
				return kf0.value + (kf1.value - kf0.value) * localT;

			case animation_interpolation::step:
				return kf0.value;

			default:
				return vector3::zero;
			}
		}

		return vector3::zero;
	}

	void animation_channel_q::create_from_loaded(const animation_channel_q_loaded& loaded, chunk_allocator32& alloc)
	{
		interpolation = loaded.interpolation;
		node_index	  = loaded.node_index;

		const uint32 kf_count		  = static_cast<uint32>(loaded.keyframes.size());
		const uint32 kf_splines_count = static_cast<uint32>(loaded.keyframes_spline.size());

		if (kf_count != 0)
		{
			keyframes					 = alloc.allocate<animation_keyframe_q>(kf_count);
			animation_keyframe_q* ptr_kf = alloc.get<animation_keyframe_q>(keyframes);
			SFG_MEMCPY(ptr_kf, loaded.keyframes.data(), keyframes.size);
		}

		if (kf_splines_count != 0)
		{
			keyframes_spline					= alloc.allocate<animation_keyframe_q_spline>(kf_splines_count);
			animation_keyframe_q_spline* ptr_kf = alloc.get<animation_keyframe_q_spline>(keyframes_spline);
			SFG_MEMCPY(ptr_kf, loaded.keyframes_spline.data(), keyframes_spline.size);
		}
	}

	void animation_channel_q::destroy(chunk_allocator32& alloc)
	{
		if (keyframes.size != 0)
			alloc.free(keyframes);

		if (keyframes_spline.size != 0)
			alloc.free(keyframes_spline);
	}

	quat animation_channel_q::sample(float time, chunk_allocator32& alloc) const
	{
		if (keyframes.size == 0 && keyframes_spline.size == 0)
			return quat::identity;

		animation_keyframe_q*		 ptr					= alloc.get<animation_keyframe_q>(keyframes);
		animation_keyframe_q_spline* ptr_spline				= alloc.get<animation_keyframe_q_spline>(keyframes_spline);
		const uint32				 keyframes_count		= keyframes.size == 0 ? 0 : sizeof(animation_keyframe_q) / keyframes.size;
		const uint32				 keyframes_spline_count = keyframes.size == 0 ? 0 : sizeof(animation_keyframe_q_spline) / keyframes_spline.size;

		if (interpolation == animation_interpolation::cubic_spline)
		{
			if (keyframes_spline_count == 0)
				return quat::identity;

			const animation_keyframe_q_spline& front = ptr_spline[0];
			const animation_keyframe_q_spline& back	 = ptr_spline[keyframes_spline_count - 1];

			if (time <= front.time)
				return front.value;
			if (time >= back.time)
				return back.value;

			size_t i = 0;
			while (i < keyframes_spline_count - 1 && time > ptr_spline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = ptr_spline[i];
			const auto& kf1 = ptr_spline[i + 1];

			float t0 = kf0.time;
			float t1 = kf1.time;

			const quat& q0			= kf0.value;
			const quat& q1			= kf1.value;
			const quat& tangentIn0	= kf0.in_tangent;
			const quat& tangentOut0 = kf0.out_tangent;

			float localT = (time - t0) / (t1 - t0);
			float t		 = localT;
			float t2	 = t * t;
			float t3	 = t2 * t;

			float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
			float h10 = t3 - 2.0f * t2 + t;
			float h01 = -2.0f * t3 + 3.0f * t2;
			float h11 = t3 - t2;

			return h00 * q0 + h10 * tangentOut0 * (t1 - t0) + h01 * q1 + h11 * tangentIn0 * (t1 - t0);
		}
		else
		{
			if (keyframes_count == 0)
				return quat::identity;

			const animation_keyframe_q& front = ptr[0];
			const animation_keyframe_q& back  = ptr[keyframes_count - 1];

			if (time <= front.time)
				return front.value;
			if (time >= back.time)
				return back.value;

			size_t i = 0;
			while (i < keyframes_count - 1 && time > ptr[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = ptr[i];
			const auto& kf1 = ptr[i + 1];

			float t0 = kf0.time;
			float t1 = kf1.time;

			const quat& q0 = kf0.value;
			const quat& q1 = kf1.value;

			float localT = (time - t0) / (t1 - t0);

			switch (interpolation)
			{
			case animation_interpolation::linear:
				return quat::slerp(q0, q1, localT);

			case animation_interpolation::step:
				return q0;

			default:
				return quat::identity;
			}
		}
	}

	void animation::create_from_loaded(const animation_loaded& loaded, chunk_allocator32& alloc)
	{
		if (!loaded.name.empty())
		{
			_name = alloc.allocate<uint8>(loaded.name.size());
			strcpy((char*)alloc.get(_name.head), loaded.name.data());
		}

		_duration = loaded.duration;

		const uint32 position_count = static_cast<uint32>(loaded.position_channels.size());

		if (position_count != 0)
		{
			_position_channels		  = alloc.allocate<animation_channel_v3>(position_count);
			animation_channel_v3* ptr = alloc.get<animation_channel_v3>(_position_channels);

			for (uint32 i = 0; i < position_count; i++)
			{
				const animation_channel_v3_loaded& ch = loaded.position_channels[i];
				animation_channel_v3&			   rt = ptr[i];
				rt.create_from_loaded(ch, alloc);
			}
		}

		const uint32 rotation_count = static_cast<uint32>(loaded.rotation_channels.size());

		if (rotation_count != 0)
		{
			_rotation_channels		 = alloc.allocate<animation_channel_q>(rotation_count);
			animation_channel_q* ptr = alloc.get<animation_channel_q>(_rotation_channels);

			for (uint32 i = 0; i < rotation_count; i++)
			{
				const animation_channel_q_loaded& ch = loaded.rotation_channels[i];
				animation_channel_q&			  rt = ptr[i];
				rt.create_from_loaded(ch, alloc);
			}
		}
		const uint32 scale_count = static_cast<uint32>(loaded.scale_channels.size());
		if (scale_count != 0)
		{

			_scale_channels			  = alloc.allocate<animation_channel_v3>(scale_count);
			animation_channel_v3* ptr = alloc.get<animation_channel_v3>(_scale_channels);

			for (uint32 i = 0; i < scale_count; i++)
			{
				const animation_channel_v3_loaded& ch = loaded.scale_channels[i];
				animation_channel_v3&			   rt = ptr[i];
				rt.create_from_loaded(ch, alloc);
			}
		}

		_position_count = static_cast<uint16>(position_count);
		_rotation_count = static_cast<uint16>(rotation_count);
		_scale_count	= static_cast<uint16>(scale_count);
	}

	void animation::destroy(chunk_allocator32& alloc)
	{
		if (_name.size != 0)
			alloc.free(_name);

		if (_position_channels.size != 0)
		{
			animation_channel_v3* ptr = alloc.get<animation_channel_v3>(_position_channels);
			for (uint16 i = 0; i < _position_count; i++)
			{
				animation_channel_v3& ch = ptr[i];
				ch.destroy(alloc);
			}
		}

		if (_rotation_channels.size != 0)
		{
			animation_channel_q* ptr = alloc.get<animation_channel_q>(_rotation_channels);
			for (uint16 i = 0; i < _rotation_count; i++)
			{
				animation_channel_q& ch = ptr[i];
				ch.destroy(alloc);
			}
		}

		if (_scale_channels.size != 0)
		{
			animation_channel_v3* ptr = alloc.get<animation_channel_v3>(_scale_channels);
			for (uint16 i = 0; i < _scale_count; i++)
			{
				animation_channel_v3& ch = ptr[i];
				ch.destroy(alloc);
			}
		}

		_name			   = {};
		_position_channels = {};
		_rotation_channels = {};
		_scale_channels	   = {};
		_position_count = _rotation_count = _scale_count = 0;
	}

}
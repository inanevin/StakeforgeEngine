// Copyright (c) 2025 Inan Evin

#include "animation.hpp"

namespace SFG
{
	vector3 animation_channel_v3::sample(float time) const
	{
		if (keyframes.empty() && keyframes_spline.empty())
			return vector3::zero; // Return a default value.

		if (interpolation == animation_interpolation::cubic_spline && !keyframes_spline.empty())
		{
			if (time <= keyframes_spline.front().time)
				return keyframes_spline.front().value;
			if (time >= keyframes_spline.back().time)
				return keyframes_spline.back().value;

			size_t i = 0;
			while (i < keyframes_spline.size() - 1 && time > keyframes_spline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes_spline[i];
			const auto& kf1 = keyframes_spline[i + 1];

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
		else if (!keyframes.empty())
		{
			if (time <= keyframes.front().time)
				return keyframes.front().value;
			if (time >= keyframes.back().time)
				return keyframes.back().value;

			size_t i = 0;
			while (i < keyframes.size() - 1 && time > keyframes[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes[i];
			const auto& kf1 = keyframes[i + 1];

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

	quat animation_channel_q::sample(float time) const
	{
		if (keyframes.empty() && keyframes_spline.empty())
			return quat::identity;

		if (interpolation == animation_interpolation::cubic_spline)
		{
			if (keyframes_spline.empty())
				return quat::identity;

			if (time <= keyframes_spline.front().time)
				return keyframes_spline.front().value;
			if (time >= keyframes_spline.back().time)
				return keyframes_spline.back().value;

			size_t i = 0;
			while (i < keyframes_spline.size() - 1 && time > keyframes_spline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes_spline[i];
			const auto& kf1 = keyframes_spline[i + 1];

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
			if (keyframes.empty())
				return quat::identity;

			if (time <= keyframes.front().time)
				return keyframes.front().value;
			if (time >= keyframes.back().time)
				return keyframes.back().value;

			size_t i = 0;
			while (i < keyframes.size() - 1 && time > keyframes[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes[i];
			const auto& kf1 = keyframes[i + 1];

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
}
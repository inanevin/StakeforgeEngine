// Copyright (c) 2025 Inan Evin

#include "easing.hpp"
#include "math.hpp"

namespace SFG
{
	float easing::smooth_damp(float current, float target, float* current_velocity, float smooth_time, float maxSpeed, float dt)
	{
		smooth_time		  = math::max(0.0001f, smooth_time);
		float num		  = 2.0f / smooth_time;
		float num2		  = num * dt;
		float num3		  = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
		float num4		  = current - target;
		float num5		  = target;
		float num6		  = maxSpeed * smooth_time;
		num4			  = math::clamp(num4, -num6, num6);
		target			  = current - num4;
		float num7		  = (*current_velocity + num * num4) * dt;
		*current_velocity = (*current_velocity - num * num7) * num3;
		float num8		  = target + (num4 + num7) * num3;
		if (num5 - current > 0.0f == num8 > num5)
		{
			num8			  = num5;
			*current_velocity = (num8 - num5) / dt;
		}
		return num8;
	}

	float easing::lerp(float val1, float val2, float amt)
	{
		return (val1 * (1.0f - amt) + val2 * amt);
	}

	float easing::cubic_lerp(float val1, float val2, float amt)
	{
		return lerp(val1, val2, 3 * amt * amt - 2 * amt * amt * amt);
	}

	float easing::cubic_interp(float val0, float val1, float val2, float val3, float amt)
	{
		float amt2 = amt * amt;
		return ((val3 * (1.0f / 2.0f) - val2 * (3.0f / 2.0f) - val0 * (1.0f / 2.0f) + val1 * (3.0f / 2.0f)) * amt * amt2 + (val0 - val1 * (5.0f / 2.0f) + val2 * 2.0f - val3 * (1.0f / 2.0f)) * amt2 + (val2 * (1.0f / 2.0f) - val0 * (1.0f / 2.0f)) * amt + val1);
	}

	float easing::cubic_interp_tangents(float val1, float tan1, float val2, float tan2, float amt)
	{
		float amt2 = amt * amt;
		return ((tan2 - val2 * 2.0f + tan1 + val1 * (2.0f)) * amt * amt2 + (tan1 * 2.0f - val1 * 3.0f + val2 * 3.0f - tan2 * 2.0f) * amt2 + tan1 * amt + val1);
	}

	float easing::bilerp(float val00, float val10, float val01, float val11, float amtX, float amtY)
	{
		return lerp(lerp(val00, val10, amtX), lerp(val01, val11, amtX), amtY);
	}

	float easing::step(float edge, float x)
	{
		return x < edge ? 0.0f : 1.0f;
	}

	float easing::ease_in(float start, float end, float alpha)
	{
		return lerp(start, end, alpha * alpha);
	}

	float easing::ease_out(float start, float end, float alpha)
	{
		return lerp(start, end, 1.0f - (1.0f - alpha) * (1.0f - alpha));
	}

	float easing::ease_in_out(float start, float end, float alpha)
	{
		if (alpha < 0.5f)
			return lerp(start, end, 2.0f * alpha * alpha);
		return lerp(start, end, 1.0f - math::fast_pow(-2.0f * alpha + 2.0f, 2.0f) / 2.0f);
	}

	float easing::cubic(float start, float end, float alpha)
	{
		return lerp(start, end, alpha * alpha * alpha);
	}

	float easing::exponential(float start, float end, float alpha)
	{
		if (alpha < 0.001f && alpha > -0.001f)
			return 0.0f;

		return lerp(start, end, math::fast_pow(2.0f, 10.0f * alpha - 10.0f));
	}

	float easing::bounce(float start, float end, float alpha)
	{
		if (alpha < (1.0f / 2.75f))
		{
			return lerp(start, end, 7.5625f * alpha * alpha);
		}
		else if (alpha < (2.0f / 2.75f))
		{
			alpha -= (1.5f / 2.75f);
			return lerp(start, end, 7.5625f * alpha * alpha + 0.75f);
		}
		else if (alpha < (2.5f / 2.75f))
		{
			alpha -= (2.25f / 2.75f);
			return lerp(start, end, 7.5625f * alpha * alpha + 0.9375f);
		}
		else
		{
			alpha -= (2.625f / 2.75f);
			return lerp(start, end, 7.5625f * alpha * alpha + 0.984375f);
		}
	}

	float easing::sinusodial(float start, float end, float alpha)
	{
		return lerp(start, end, -math::cos(alpha * MATH_PI) / 2.0f + 0.5f);
	};

} // namespace SFG

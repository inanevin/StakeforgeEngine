/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Math/Easing.hpp"
#include "SFG/Math/Math.hpp"

namespace SFG
{
	float Easing::SmoothDamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
	{
		smoothTime		 = Math::Max(0.0001f, smoothTime);
		float num		 = 2.0f / smoothTime;
		float num2		 = num * deltaTime;
		float num3		 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
		float num4		 = current - target;
		float num5		 = target;
		float num6		 = maxSpeed * smoothTime;
		num4			 = Math::Clamp(num4, -num6, num6);
		target			 = current - num4;
		float num7		 = (*currentVelocity + num * num4) * deltaTime;
		*currentVelocity = (*currentVelocity - num * num7) * num3;
		float num8		 = target + (num4 + num7) * num3;
		if (num5 - current > 0.0f == num8 > num5)
		{
			num8			 = num5;
			*currentVelocity = (num8 - num5) / deltaTime;
		}
		return num8;
	}

	float Easing::Lerp(float val1, float val2, float amt)
	{
		return (val1 * (1.0f - amt) + val2 * amt);
	}

	float Easing::CubicLerp(float val1, float val2, float amt)
	{
		return Lerp(val1, val2, 3 * amt * amt - 2 * amt * amt * amt);
	}

	float Easing::CubicInterpolation(float val0, float val1, float val2, float val3, float amt)
	{
		float amt2 = amt * amt;
		return ((val3 * (1.0f / 2.0f) - val2 * (3.0f / 2.0f) - val0 * (1.0f / 2.0f) + val1 * (3.0f / 2.0f)) * amt * amt2 + (val0 - val1 * (5.0f / 2.0f) + val2 * 2.0f - val3 * (1.0f / 2.0f)) * amt2 + (val2 * (1.0f / 2.0f) - val0 * (1.0f / 2.0f)) * amt + val1);
	}

	float Easing::CubicInterpolationSpecifyTangents(float val1, float tan1, float val2, float tan2, float amt)

	{
		float amt2 = amt * amt;
		return ((tan2 - val2 * 2.0f + tan1 + val1 * (2.0f)) * amt * amt2 + (tan1 * 2.0f - val1 * 3.0f + val2 * 3.0f - tan2 * 2.0f) * amt2 + tan1 * amt + val1);
	}

	float Easing::BiLerp(float val00, float val10, float val01, float val11, float amtX, float amtY)
	{
		return Lerp(Lerp(val00, val10, amtX), Lerp(val01, val11, amtX), amtY);
	}

	float Easing::Step(float edge, float x)
	{
		return x < edge ? 0.0f : 1.0f;
	}

	float Easing::EaseIn(float start, float end, float alpha)
	{
		return Lerp(start, end, alpha * alpha);
	}

	float Easing::EaseOut(float start, float end, float alpha)
	{
		return Lerp(start, end, 1.0f - (1.0f - alpha) * (1.0f - alpha));
	}

	float Easing::EaseInOut(float start, float end, float alpha)
	{
		if (alpha < 0.5f)
			return Lerp(start, end, 2.0f * alpha * alpha);
		return Lerp(start, end, 1.0f - FastPow::Pow(-2.0f * alpha + 2.0f, 2.0f) / 2.0f);
	}

	float Easing::Cubic(float start, float end, float alpha)
	{
		return Lerp(start, end, alpha * alpha * alpha);
	}

	float Easing::Exponential(float start, float end, float alpha)
	{
		if (alpha < 0.001f && alpha > -0.001f)
			return 0.0f;

		return Lerp(start, end, FastPow::Pow(2.0f, 10.0f * alpha - 10.0f));
	}

	float Easing::Bounce(float start, float end, float alpha)
	{
		if (alpha < (1.0f / 2.75f))
		{
			return Lerp(start, end, 7.5625f * alpha * alpha);
		}
		else if (alpha < (2.0f / 2.75f))
		{
			alpha -= (1.5f / 2.75f);
			return Lerp(start, end, 7.5625f * alpha * alpha + 0.75f);
		}
		else if (alpha < (2.5f / 2.75f))
		{
			alpha -= (2.25f / 2.75f);
			return Lerp(start, end, 7.5625f * alpha * alpha + 0.9375f);
		}
		else
		{
			alpha -= (2.625f / 2.75f);
			return Lerp(start, end, 7.5625f * alpha * alpha + 0.984375f);
		}
	}

	float Easing::Sinusodial(float start, float end, float alpha)
	{
		return Lerp(start, end, -Math::Cos(alpha * MATH_PI) / 2.0f + 0.5f);
	};

} // namespace SFG

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

#include "SFG/Math/Math.hpp"

namespace SFG
{
	float Math::SmoothDamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
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

	float Math::Decimal(float val, int decimals)
	{
		if (decimals == 0)
			return static_cast<float>(static_cast<int>(val));

		float multiplier = static_cast<float>(std::pow(10.0f, decimals));
		return std::floor(val * multiplier) / multiplier;
	}
} // namespace SFG

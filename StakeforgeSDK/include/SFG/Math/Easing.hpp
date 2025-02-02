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

#pragma once

#include "SFG/Math/FastPow.hpp"

namespace SFG
{
	class Easing
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="current"></param>
		/// <param name="target"></param>
		/// <param name="currentVelocity"></param>
		/// <param name="smoothTime"></param>
		/// <param name="maxSpeed"></param>
		/// <param name="deltaTime"></param>
		/// <returns></returns>
		static float SmoothDamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		static float Lerp(float val1, float val2, float amt);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		static float CubicLerp(float val1, float val2, float amt);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val0"></param>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="val3"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		static float CubicInterpolation(float val0, float val1, float val2, float val3, float amt);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val1"></param>
		/// <param name="tan1"></param>
		/// <param name="val2"></param>
		/// <param name="tan2"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		static float CubicInterpolationSpecifyTangents(float val1, float tan1, float val2, float tan2, float amt);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val00"></param>
		/// <param name="val10"></param>
		/// <param name="val01"></param>
		/// <param name="val11"></param>
		/// <param name="amtX"></param>
		/// <param name="amtY"></param>
		/// <returns></returns>
		static float BiLerp(float val00, float val10, float val01, float val11, float amtX, float amtY);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="edge"></param>
		/// <param name="x"></param>
		/// <returns></returns>
		static float Step(float edge, float x);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float EaseIn(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float EaseOut(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float EaseInOut(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float Cubic(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float Exponential(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float Bounce(float start, float end, float alpha);

		/// <summary>
		///
		/// </summary>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		static float Sinusodial(float start, float end, float alpha);
	};
} // namespace SFG

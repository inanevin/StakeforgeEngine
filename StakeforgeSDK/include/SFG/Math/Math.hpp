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

#include "SFG/Type/SizeDefinitions.hpp"
#include <cmath>
#include <cstdlib>
#include <random>

namespace SFG
{

#define DEG_2_RAD 0.0174533f
#define RAD_2_DEG 57.2958f

#define MATH_PI		   3.1415926535897932f
#define MATH_TWO_PI	   6.28318530717959f
#define MATH_HALF_PI   1.57079632679f
#define MATH_R_PI	   0.31830988618f
#define MATH_R_TWO_PI  0.159154943091895f
#define MATH_R_HALF_PI 0.636619772367581f

#define MATH_E		2.71828182845904523536f
#define MATH_R_LN_2 1.44269504088896f

	class Math
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
		/// <param name="val"></param>
		/// <param name="decimals"></param>
		/// <returns></returns>
		static float Decimal(float val, int decimals);

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline int32 TruncToInt(float val)
		{
			return (int32)val;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float TruncToFloat(float val)
		{
			return (float)TruncToInt(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline int32 FloorToInt(float val)
		{
			return TruncToInt(FloorToFloat(val));
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float FloorToFloat(float val)
		{
			return floorf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float FloorToFloatEven(float val)
		{
			const float v = floorf(val);
			return (int)v % 2 == 0 ? v : v - 1.0f;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline double FloorToDouble(double val)
		{
			return floor(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline int32 RoundToInt(float val)
		{
			return CeilToInt(val - 0.5f);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline int32 RoundToIntEven(float val)
		{
			const int res = CeilToInt(val - 0.5f);
			return res % 2 == 0 ? res : res - 1;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float RoundToFloat(float val)
		{
			return CeilToFloat(val - 0.5f);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline int32 CeilToInt(float val)
		{
			return TruncToInt(ceilf(val));
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float CeilToFloat(float val)
		{
			return ceilf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline double CeilToDouble(double val)
		{
			return ceil(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float Fractional(float val)
		{
			return val - TruncToFloat(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float Exp(float val)
		{
			return expf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float Ln(float val)
		{
			return logf(val);
		}

		static inline float FAbs(float f)
		{
			return fabs(f);
		}

		static inline float CopySign(float num, float sign)
		{
			return copysignf(num, sign);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="base"></param>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline float Logx(float base, float val)
		{
			return Ln(val) / Ln(base);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Log2(float val)
		{
			return Ln(val) * MATH_R_LN_2;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="num"></param>
		/// <param name="den"></param>
		/// <returns></returns>
		static float inline Fmod(float num, float den)
		{
			return fmodf(num, den);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Sin(float val)
		{
			return sinf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Asin(float val)
		{
			return asinf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Cos(float val)
		{
			return cosf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Acos(float val)
		{
			return acosf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Tan(float val)
		{
			return tanf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Atan(float val)
		{
			return atanf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <param name="val2"></param>
		/// <returns></returns>
		static float inline Atan(float val, float val2)
		{
			return atan2f(val, val2);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <param name="val2"></param>
		/// <returns></returns>
		static float inline Atan2(float val, float val2)
		{
			return atan2(val, val2);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <param name="power"></param>
		/// <returns></returns>
		static float inline Pow(float val, float power)
		{
			return powf(val, power);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Sqrt(float val)
		{
			return sqrtf(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline Reciprocal(float val)
		{
			return 1.0f / val;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static float inline RSqrt(float val)
		{
			return Reciprocal(sqrtf(val));
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline bool IsNaN(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7FFFFFFF) > 0x7F800000;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline bool IsFinite(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7F800000) != 0x7F800000;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static inline int32 Rand()
		{
			return ::rand();
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		static inline int32 Rand(int32 min, int32 max)
		{
			return (int32)Lerp((float)min, (float)max, RandF());
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="seed"></param>
		static inline void SeedRand(int32 seed)
		{
			srand((uint32)seed);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static inline float RandF()
		{
			return ::rand() / (float)RAND_MAX;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="Min"></param>
		/// <param name="Max"></param>
		/// <returns></returns>
		static inline float RandF(float Min, float Max)
		{
			return Lerp(Min, Max, RandF());
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline uint32 FloorLog2(uint32 val)
		{
			uint32 pos = 0;
			if (val >= 1 << 16)
			{
				val >>= 16;
				pos += 16;
			}
			if (val >= 1 << 8)
			{
				val >>= 8;
				pos += 8;
			}
			if (val >= 1 << 4)
			{
				val >>= 4;
				pos += 4;
			}
			if (val >= 1 << 2)
			{
				val >>= 2;
				pos += 2;
			}
			if (val >= 1 << 1)
			{
				pos += 1;
			}
			return (val == 0) ? 0 : pos;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline uint32 GetNumLeadingZeroes(uint32 val)
		{
			if (val == 0)
			{
				return 32;
			}
			return 31 - FloorLog2(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline uint32 CeilLog2(uint32 val)
		{
			if (val <= 1)
			{
				return 0;
			}
			return FloorLog2(val - 1) + 1;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		static inline uint32 RoundUpToNextPowerOf2(uint32 val)
		{
			return 1 << CeilLog2(val);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="cmp"></param>
		/// <param name="valIfGreaterOrEqualToZero"></param>
		/// <param name="valIfLessZero"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Select(const T& cmp, const T& valIfGreaterOrEqualToZero, const T& valIfLessZero)
		{
			return cmp >= (T)(0) ? valIfGreaterOrEqualToZero : valIfLessZero;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Abs(const T& val)
		{
			return val >= (T)(0) ? val : -val;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Min(const T& val1, const T& val2)
		{
			return val1 <= val2 ? val1 : val2;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="val3"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Min3(const T& val1, const T& val2, const T& val3)
		{
			return Min(Min(val1, val2), val3);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Max(const T& val1, const T& val2)
		{
			return val1 >= val2 ? val1 : val2;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="val3"></param>
		/// <returns></returns>
		template <typename T> static constexpr T Max3(const T& val1, const T& val2, const T& val3)
		{
			return Max(Max(val1, val2), val3);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="val3"></param>
		/// <returns></returns>
		template <typename T> static T Mad(const T& val1, const T& val2, const T& val3)
		{
			return val1 * val2 + val3;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		template <typename T, typename U> static T Lerp(const T& val1, const T& val2, const U& amt)
		{
			return (T)(val1 * ((U)(1) - amt) + val2 * amt);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="U"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="amt"></param>
		/// <returns></returns>
		template <typename T, typename U> static T CubicLerp(const T& val1, const T& val2, const U& amt)
		{
			return Lerp(val1, val2, 3 * amt * amt - 2 * amt * amt * amt);
		}

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
		template <typename T, typename U> static T CubicInterpolation(const T& val0, const T& val1, const T& val2, const T& val3, const U& amt)
		{
			U amt2 = amt * amt;
			return ((val3 * (U(1) / U(2)) - val2 * (U(3) / U(2)) - val0 * (U(1) / U(2)) + val1 * (U(3) / U(2))) * amt * amt2 + (val0 - val1 * (U(5) / U(2)) + val2 * U(2) - val3 * (U(1) / U(2))) * amt2 + (val2 * (U(1) / U(2)) - val0 * (U(1) / U(2))) * amt +
					val1);
		}

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
		template <typename T, typename U> static T CubicInterpolationSpecifyTangents(const T& val1, const T& tan1, const T& val2, const T& tan2, const U& amt)
		{
			U amt2 = amt * amt;
			return ((tan2 - val2 * U(2) + tan1 + val1 * (U(2))) * amt * amt2 + (tan1 * U(2) - val1 * U(3) + val2 * U(3) - tan2 * U(2)) * amt2 + tan1 * amt + val1);
		}

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
		template <typename T, typename U> static T BiLerp(const T& val00, const T& val10, const T& val01, const T& val11, const U& amtX, const U& amtY)
		{
			return Lerp(Lerp(val00, val10, amtX), Lerp(val01, val11, amtX), amtY);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <param name="Min"></param>
		/// <param name="Max"></param>
		/// <returns></returns>
		template <typename T> static T Clamp(const T& val, const T& Min, const T& Max)
		{
			if (val > Max)
			{
				return Max;
			}
			else if (val > Min)
			{
				return val;
			}
			else
			{
				return Min;
			}
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <param name="fromLow"></param>
		/// <param name="fromHigh"></param>
		/// <param name="toLow"></param>
		/// <param name="toHigh"></param>
		/// <returns></returns>
		template <typename T> static T Remap(const T& val, const T& fromLow, const T& fromHigh, const T& toLow, const T& toHigh)
		{
			return toLow + (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <returns></returns>
		template <typename T> static T Saturate(const T& val)
		{
			return Clamp(val, (T)(0), (T)(1));
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val1"></param>
		/// <param name="val2"></param>
		/// <param name="errorMargin"></param>
		/// <returns></returns>
		template <typename T> static bool Equals(const T& val1, const T& val2, const T& errorMargin)
		{
			return Abs(val1 - val2) < errorMargin;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="val"></param>
		/// <returns></returns>
		template <typename T> static bool IsZero(const T& val)
		{
			return Abs(val - (T)0) <= (T)0.00001f;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="edge"></param>
		/// <param name="x"></param>
		/// <returns></returns>
		template <typename T> static T Step(T edge, T x)
		{
			return x < edge ? 0.0f : 1.0f;
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T Linear(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha);
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T EaseIn(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha * alpha);
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T EaseOut(T start, T end, float alpha)
		{
			return Lerp(start, end, 1.0f - (1.0f - alpha) * (1.0f - alpha));
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T EaseInOut(T start, T end, float alpha)
		{
			if (alpha < 0.5f)
				return Lerp(start, end, 2.0f * alpha * alpha);
			return Lerp(start, end, 1.0f - Pow(-2.0f * alpha + 2.0f, 2.0f) / 2.0f);
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T Cubic(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha * alpha * alpha);
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T Sinusodial(T start, T end, float alpha)
		{
			return Lerp(start, end, -Cos(alpha * MATH_PI) / 2.0f + 0.5f);
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T Exponential(T start, T end, float alpha)
		{
			return Lerp(start, end, Equals(alpha, 0.0f, 0.001f) ? 0.0f : Pow(2.0f, 10.0f * alpha - 10.0f));
		};

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="alpha"></param>
		/// <returns></returns>
		template <typename T> static T Bounce(T start, T end, float alpha)
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
		};

		// TODO: Min and max of array
	};
} // namespace SFG

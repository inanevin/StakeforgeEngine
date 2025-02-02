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
		static float FloorToFloat(float val)
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
		/// <returns></returns>
		template <typename T> static constexpr T Max(const T& val1, const T& val2)
		{
			return val1 >= val2 ? val1 : val2;
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
	};
} // namespace SFG

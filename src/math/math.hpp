// Copyright (c) 2025 Inan Evin
#pragma once

#include <cmath>
#include "math_common.hpp"

#undef min
#undef max

namespace SFG
{
	namespace math
	{

		unsigned int floor_log2(unsigned int val);
		double		 fast_pow(double base, double exponent);

		template <typename T> inline T clamp(T value, T min_val, T max_val)
		{
			return std::fmax(min_val, std::fmin(value, max_val));
		}
		template <typename T> inline T max(T a, T b)
		{
			return std::fmax(a, b);
		}
		template <typename T> inline T min(T a, T b)
		{
			return std::fmin(a, b);
		}
		template <typename T> inline T abs(T value)
		{
			return std::fabs(value);
		}
		template <typename T> inline T pow(T base, T exponent)
		{
			return std::pow(base, exponent);
		}
		template <typename T> inline T degrees_to_radians(T degrees)
		{
			return degrees * DEG_2_RAD;
		}
		template <typename T> inline T radians_to_degrees(T radians)
		{
			return radians * RAD_2_DEG;
		}
		template <typename T> inline bool almost_equal(T a, T b, T epsilon = MATH_EPS)
		{
			return abs(a - b) < epsilon;
		}
		template <typename T> inline int sign(T value)
		{
			return (T(0) < value) - (value < T(0));
		}
		template <typename T> inline T lerp(T a, T b, T t)
		{
			return a + t * (b - a);
		}
		template <typename T> inline T inverse_lerp(T a, T b, T value)
		{
			if (std::fabs(b - a) < MATH_EPS)
				return T(0);
			return (value - a) / (b - a);
		}

		template <typename T> inline T remap(T value, T in_min, T in_max, T out_min, T out_max)
		{
			if (std::fabs(in_max - in_min) < MATH_EPS)
				return out_min;
			T normalized_value = (value - in_min) / (in_max - in_min);
			return out_min + normalized_value * (out_max - out_min);
		}

		inline float modf(float value, float* integral_part)
		{
			return std::modf(value, integral_part);
		}
		inline double modf(double value, double* integral_part)
		{
			return std::modf(value, integral_part);
		}
		inline float cos(float angle_rad)
		{
			return std::cos(angle_rad);
		}
		inline double cos(double angle_rad)
		{
			return std::cos(angle_rad);
		}
		inline float sin(float angle_rad)
		{
			return std::sin(angle_rad);
		}
		inline double sin(double angle_rad)
		{
			return std::sin(angle_rad);
		}
		inline float round(float value)
		{
			return std::round(value);
		}
		inline double round(double value)
		{
			return std::round(value);
		}
		inline float sqrt(float value)
		{
			return std::sqrtf(value);
		}
		inline float tan(float value)
		{
			return std::tanf(value);
		}
	}
}
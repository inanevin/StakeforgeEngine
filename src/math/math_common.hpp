// Copyright (c) 2025 Inan Evin
#pragma once

#include <limits>

namespace SFG
{
	namespace math
	{
#define DEG_2_RAD								0.0174533f
#define RAD_2_DEG								57.2958f
#define MATH_PI									3.1415926535897932f
#define MATH_TWO_PI								6.28318530717959f
#define MATH_HALF_PI							1.57079632679f
#define MATH_R_PI								0.31830988618f
#define MATH_R_TWO_PI							0.159154943091895f
#define MATH_R_HALF_PI							0.636619772367581f
#define MATH_E									2.71828182845904523536f
#define MATH_R_LN_2								1.44269504088896f
#define MATH_EPS								0.00001f
#define MATH_INF_F								std::numeric_limits<float>::infinity()
#define MATH_NAN								std::numeric_limits<float>::quiet_NaN()
#define ALIGN_SIZE_POW(sizeToAlign, PowerOfTwo) (((sizeToAlign) + (PowerOfTwo)-1) & ~((PowerOfTwo)-1))
#define ALIGN_UP(size, alignment)				(size + alignment - 1) & ~(alignment - 1)
#define SET_BIT(value, bit)						value | (1 << bit)
#define CHECK_BIT(value, bit)					(value & (1 << bit)) != 0
#define UNSET_BIT(value, bit)					value & ~(1 << bit)

	}
}
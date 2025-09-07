// Copyright (c) 2025 Inan Evin

#include "math.hpp"

namespace SFG
{
	unsigned int math::floor_log2(unsigned int val)
	{
		unsigned int pos = 0;
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

	double math::fast_pow(double a, double b)
	{
		// https://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/
		union {
			double d;
			int	   x[2];
		} u	   = {a};
		u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;
		return u.d;
	}

}

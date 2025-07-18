// Copyright (c) 2025 Inan Evin

#include "math.hpp"
// Copyright (c) 2025 Inan Evin

namespace Game
{
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

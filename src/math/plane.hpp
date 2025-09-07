// Copyright (c) 2025 Inan Evin
#pragma once

#include "vector3.hpp"

namespace SFG
{
	struct plane
	{
		plane()	 = default;
		~plane() = default;
		plane(float x, float y, float z, float dist) : normal(vector3(x, y, z)), distance(dis){};
		plane(const vector3& n, float dist) : normal(n), distance(dist){};

		void  normalize();
		float get_signed_distance(const vector3& point) const;

		vector3 normal	 = vector3::zero;
		float	distance = 0.0f;
	};

}
// Copyright (c) 2025 Inan Evin
#pragma once
#include "math/vector3.hpp"

namespace SFG
{
	struct plane;

	struct aabb
	{
		aabb() = default;
		aabb(vector3 min, vector3 max)
		{
			bounds_min		   = min;
			bounds_max		   = max;
			bounds_half_extent = (max - min) / 2.0f;
		}
		~aabb() = default;

		vector3 bounds_half_extent = vector3::zero;
		vector3 bounds_min		   = vector3::zero;
		vector3 bounds_max		   = vector3::zero;

		bool	is_inside_plane(const vector3& center, const plane& plane);
		vector3 get_positive(const vector3& normal) const;
		vector3 get_negative(const vector3& normal) const;

		inline void update_half_extents()
		{
			bounds_half_extent = (bounds_max - bounds_min) / 2.0f;
		}
	};
}
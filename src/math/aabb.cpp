// Copyright (c) 2025 Inan Evin

#include "aabb.hpp"
#include "plane.hpp"
#include "math/math.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

namespace SFG
{
	bool aabb::is_inside_plane(const vector3& center, const plane& plane)
	{
		const float r = bounds_half_extent.x * math::abs(plane.normal.x) + bounds_half_extent.y * math::abs(plane.normal.y) + bounds_half_extent.z * math::abs(plane.normal.z);
		return -r <= plane.get_signed_distance(center);
	}
	vector3 aabb::get_positive(const vector3& normal) const
	{
		vector3 positive = bounds_min;
		if (normal.x >= 0.0f)
			positive.x = bounds_max.x;
		if (normal.y >= 0.0f)
			positive.y = bounds_max.y;
		if (normal.z >= 0.0f)
			positive.z = bounds_max.z;

		return positive;
	}
	vector3 aabb::get_negative(const vector3& normal) const
	{
		vector3 negative = bounds_max;
		if (normal.x >= 0.0f)
			negative.x = bounds_min.x;
		if (normal.y >= 0.0f)
			negative.y = bounds_min.y;
		if (normal.z >= 0.0f)
			negative.z = bounds_min.z;

		return negative;
	}

	void aabb::remove(const aabb& other)
	{
		bounds_min -= other.bounds_min;
		bounds_max -= other.bounds_max;
	}

	void aabb::add(const aabb& other)
	{
		bounds_min += other.bounds_min;
		bounds_max += other.bounds_max;
	}

	void aabb::serialize(ostream& stream) const
	{
		stream << bounds_min;
		stream << bounds_max;
	}
	void aabb::deserialize(istream& stream)
	{
		stream >> bounds_min;
		stream >> bounds_max;
		update_half_extents();
	}
}

#include "plane.hpp"

namespace SFG
{
	void plane::normalize()
	{
		float len = normal.magnitude();
		if (len > MATH_EPS)
		{
			normal /= len;
			distance /= len;
		}
	}

	float plane::get_signed_distance(const vector3& point) const
	{
		return vector3::dot(normal, point) - distance;
	}
}
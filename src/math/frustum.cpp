// Copyright (c) 2025 Inan Evin
#include "frustum.hpp"
#include "aabb.hpp"
#include "matrix4x4.hpp"

namespace SFG
{
	frustum_result frustum::test(const frustum& fr, const aabb& other)
	{
		frustum_result test = frustum_result::inside;

		auto performTest = [&](const plane& p) {
			const float	  pos	 = -p.distance;
			const vector3 normal = p.normal;

			if (vector3::dot(normal, other.get_positive(normal)) + pos < 0.0f)
				test = frustum_result ::outside;

			if (vector3::dot(normal, other.get_negative(normal)) + pos < 0.0f)
				test = frustum_result::intersects;
		};

		performTest(fr.left);
		if (test == frustum_result::outside)
			return test;

		performTest(fr.right);
		if (test == frustum_result::outside)
			return test;

		performTest(fr.top);
		if (test == frustum_result::outside)
			return test;

		performTest(fr.bottom);
		if (test == frustum_result::outside)
			return test;

		performTest(fr.near);
		if (test == frustum_result::outside)
			return test;

		performTest(fr.far);

		return test;
	}

	frustum frustum::extract(const matrix4x4& m)
	{
		frustum fr = {};
		fr.left	   = plane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
		fr.right   = plane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
		fr.bottom  = plane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
		fr.top	   = plane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
		fr.near	   = plane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
		fr.far	   = plane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
		fr.left.normalize();
		fr.right.normalize();
		fr.bottom.normalize();
		fr.near.normalize();
		fr.far.normalize();
		return fr;
	}
}
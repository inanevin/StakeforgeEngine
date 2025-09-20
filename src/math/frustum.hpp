// Copyright (c) 2025 Inan Evin
#pragma once

#include "plane.hpp"

#undef near
#undef far

namespace SFG
{
	class matrix4x4;

	enum class frustum_result
	{
		inside,
		outside,
		intersects,
	};

	struct aabb;

	struct frustum
	{
		static frustum_result test(const frustum& fr, const aabb& other);
		static frustum		  extract(const matrix4x4& view_proj);

		plane left	 = {};
		plane right	 = {};
		plane bottom = {};
		plane top	 = {};
		plane near	 = {};
		plane far	 = {};
	};
}
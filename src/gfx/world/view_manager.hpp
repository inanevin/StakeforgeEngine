// Copyright (c) 2025 Inan Evin

#pragma once
#include "data/static_vector.hpp"
#include "math/frustum.hpp"
#include "math/matrix4x4.hpp"

namespace SFG
{
	class world;
	struct vector2ui16;

	struct view
	{
		matrix4x4 view_matrix	   = matrix4x4::identity;
		matrix4x4 proj_matrix	   = matrix4x4::identity;
		matrix4x4 view_proj_matrix = matrix4x4::identity;
		frustum	  view_frustum	   = {};
	};

#define MAX_VIEWS 25

	class view_manager
	{
	public:
		void init(world* w);
		void uninit();
		void generate_views(const vector2ui16& res, double interpolation);
		void reset();

		bool visibility_test_any_view(const aabb& box);
		bool visibility_test_main_view(const aabb& box);

		inline const static_vector<view, MAX_VIEWS>& get_views() const
		{
			return _views;
		}

	private:
		world*						   _world = nullptr;
		static_vector<view, MAX_VIEWS> _views;
	};
}

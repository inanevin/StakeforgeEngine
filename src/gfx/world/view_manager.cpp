// Copyright (c) 2025 Inan Evin

#include "view_manager.hpp"
#include "world/world.hpp"

namespace SFG
{
	void view_manager::init(world* w)
	{
		_views.clear();
		_world = w;
	}

	void view_manager::uninit()
	{
	}

	void view_manager::reset()
	{
		_views.clear();
	}

	void view_manager::generate_views(const vector2ui16& res, double interpolation)
	{
		const float		alpha			 = static_cast<float>(interpolation);
		camera&			cam				 = _world->get_camera();
		const vector3&	prev_pos		 = cam.get_prev_pos();
		const vector3&	current_pos		 = cam.get_pos();
		const quat&		prev_rot		 = cam.get_prev_rot();
		const quat&		current_rot		 = cam.get_rot();
		const vector3	interpolated_pos = vector3::lerp(prev_pos, current_pos, alpha);
		const quat&		interpolated_rot = quat::slerp(prev_rot, current_rot, alpha);
		const matrix4x4 view_m			 = camera::view(interpolated_rot, interpolated_pos);
		const matrix4x4 proj			 = camera::proj(cam.get_fov(), res, cam.get_near(), cam.get_far());
		const matrix4x4 view_proj		 = proj * view_m;
		cam.set_prev_position(current_pos);
		cam.set_prev_rotation(current_rot);

		const view v = {
			.view_matrix	  = view_m,
			.proj_matrix	  = proj,
			.view_proj_matrix = view_proj,
			.view_frustum	  = frustum::extract(view_proj),
		};

		_views.push_back(v);
	}

	bool view_manager::visibility_test_any_view(const aabb& box)
	{
		for (const view& v : _views)
		{
			if (frustum::test(v.view_frustum, box) == frustum_result::outside)
				return false;
		}

		return true;
	}

	bool view_manager::visibility_test_main_view(const aabb& box)
	{
		SFG_ASSERT(_views.empty());
		return frustum::test(_views[0].view_frustum, box) != frustum_result::outside;
	}
}

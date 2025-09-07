// Copyright (c) 2025 Inan Evin

#pragma once

#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "math/matrix4x4.hpp"

namespace SFG
{
	class vector2ui16;

	class camera
	{
	public:
		void update_matrices(const vector2ui16& size);

		static matrix4x4 view(const quat& rot, const vector3& pos);
		static matrix4x4 proj(float fov_degrees, const vector2ui16& size, float near, float far);
		static matrix4x4 ortho(const vector2ui16& size, float near, float far);

	private:
		matrix4x4 _view			 = {};
		matrix4x4 _proj			 = {};
		matrix4x4 _view_proj	 = {};
		matrix4x4 _view_proj_inv = {};
		quat	  _rotation		 = {};
		vector3	  _position		 = {};
		float	  _z_near		 = 0.1f;
		float	  _z_far		 = 0.1f;
		float	  _fov_degrees	 = 90.0f;
	};
}

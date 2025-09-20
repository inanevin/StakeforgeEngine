// Copyright (c) 2025 Inan Evin

#pragma once

#include "math/vector3.hpp"
#include "math/quat.hpp"
#include "math/matrix4x4.hpp"

namespace SFG
{
	struct vector2ui16;

	class camera
	{
	public:
		static matrix4x4 view(const quat& rot, const vector3& pos);
		static matrix4x4 proj(float fov_degrees, const vector2ui16& size, float near, float far);
		static matrix4x4 ortho(const vector2ui16& size, float near, float far);

		inline const quat& get_rot() const
		{
			return _rotation;
		}

		inline const quat& get_prev_rot() const
		{
			return _prev_rotation;
		}

		inline const vector3& get_pos() const
		{
			return _position;
		}

		inline const vector3& get_prev_pos() const
		{
			return _prev_position;
		}

		inline float get_near() const
		{
			return _near;
		}

		inline float get_far() const
		{
			return _far;
		}

		inline float get_fov() const
		{
			return _fov_degrees;
		}

		inline void set_position(const vector3& pos)
		{
			_position = pos;
		}

		inline void set_rotation(const quat& q)
		{
			_rotation = q;
		}

		inline void set_prev_position(const vector3& pos)
		{
			_prev_position = pos;
		}

		inline void set_prev_rotation(const quat& q)
		{
			_prev_rotation = q;
		}

		inline void set_near(float f)
		{
			_near = f;
		}

		inline void set_far(float f)
		{
			_far = f;
		}

	private:
		quat	_prev_rotation = {};
		quat	_rotation	   = {};
		vector3 _prev_position = {};
		vector3 _position	   = {};

		float _near		   = 0.1f;
		float _far		   = 0.1f;
		float _fov_degrees = 90.0f;
	};
}

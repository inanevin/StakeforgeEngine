// Copyright (c) 2025 Inan Evin
#pragma once
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/vector4i16.hpp"

namespace SFG
{
	class ostream;
	class istream;

	struct vertex_static
	{
		vector3 pos		= vector3::zero;
		vector3 normal	= vector3::zero;
		vector4 tangent = vector4::zero;
		vector2 uv		= vector2::zero;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};

	struct vertex_skinned
	{
		vector3	   pos			= vector3::zero;
		vector3	   normal		= vector3::zero;
		vector4	   tangent		= vector4::zero;
		vector2	   uv			= vector2::zero;
		vector4	   bone_weights = vector4::zero;
		vector4i16 bone_indices = vector4i16::zero;

		void serialize(ostream& stream) const;
		void deserialize(istream& stream);
	};
}
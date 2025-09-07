// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace SFG
{

	class vector4i
	{
	public:
		vector4i(){};
		vector4i(int32 _x, int32 _y, int32 _z, int32 _w) : x(_x), y(_y), z(_z), w(_w){};

		static vector4i zero;
		static vector4i one;

		vector4i operator-(const vector4i& other) const
		{
			return vector4i(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		vector4i operator+(const vector4i& other) const
		{
			return vector4i(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		int32 x = 0;
		int32 y = 0;
		int32 z = 0;
		int32 w = 0;
	};

}
// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace SFG
{

	class vector4i16
	{
	public:
		vector4i16(){};
		vector4i16(int16 _x, int16 _y, int16 _z, int16 _w) : x(_x), y(_y), z(_z), w(_w){};

		static vector4i16 zero;
		static vector4i16 one;

		vector4i16 operator-(const vector4i16& other) const
		{
			return vector4i16(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		vector4i16 operator+(const vector4i16& other) const
		{
			return vector4i16(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		int16 x = 0;
		int16 y = 0;
		int16 z = 0;
		int16 w = 0;
	};

}
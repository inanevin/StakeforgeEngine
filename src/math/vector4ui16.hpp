// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace Game
{
	class vector4ui16
	{
	public:
		vector4ui16(){};
		vector4ui16(uint16 _x, uint16 _y, uint16 _z, uint16 _w) : x(_x), y(_y), z(_z), w(_w){};

		static vector4ui16 zero;
		static vector4ui16 one;

		uint16 x = 0;
		uint16 y = 0;
		uint16 z = 0;
		uint16 w = 0;
	};

}
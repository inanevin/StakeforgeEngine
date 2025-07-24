// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace Game
{
	class vector4ui
	{
	public:
		vector4ui(){};
		vector4ui(uint32 _x, uint32 _y, uint32 _z, uint32 _w) : x(_x), y(_y), z(_z), w(_w){};

		static vector4ui zero;
		static vector4ui one;

		uint32 x = 0;
		uint32 y = 0;
		uint32 z = 0;
		uint32 w = 0;
	};

}
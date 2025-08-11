// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace SFG
{
	class vector2i;

	class vector2ui
	{
	public:
		vector2ui(){};
		vector2ui(uint32 _x, uint32 _y) : x(_x), y(_y){};
		vector2ui(const vector2i& v);

		static vector2ui zero;
		static vector2ui one;

		uint32 x = 0;
		uint32 y = 0;
	};

}
// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace Game
{
	class vector2ui;

	class vector2i
	{
	public:
		vector2i(){};
		vector2i(int32 _x, int32 _y) : x(_x), y(_y){};
		vector2i(const vector2ui& v);

		static vector2i zero;
		static vector2i one;

		static vector2i clamp(const vector2i& v, const vector2i& min, const vector2i& max);

		vector2i operator-(const vector2i& other) const
		{
			return vector2i(x - other.x, y - other.y);
		}

		vector2i operator+(const vector2i& other) const
		{
			return vector2i(x + other.x, y + other.y);
		}

		int32 x = 0;
		int32 y = 0;
	};

}
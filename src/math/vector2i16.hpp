// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"

namespace Game
{

	class vector2i16
	{
	public:
		vector2i16(){};
		vector2i16(int16 _x, int16 _y) : x(_x), y(_y){};

		static vector2i16 zero;
		static vector2i16 one;

		static vector2i16 clamp(const vector2i16& v, const vector2i16& min, const vector2i16& max);

		vector2i16 operator-(const vector2i16& other) const
		{
			return vector2i16(x - other.x, y - other.y);
		}

		vector2i16 operator+(const vector2i16& other) const
		{
			return vector2i16(x + other.x, y + other.y);
		}

		int16 x = 0;
		int16 y = 0;
	};

}
// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/string_view.hpp"

namespace Game
{
	class ostream;
	class istream;

	class serialization
	{
	public:
		static bool	   write_to_file(string_view fileInput, const char* targetFilePath);
		static bool	   save_to_file(const char* path, ostream& stream);
		static istream load_from_file(const char* path);
	};

}

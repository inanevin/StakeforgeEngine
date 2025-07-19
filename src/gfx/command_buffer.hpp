// Copyright (c) 2025 Inan Evin
#pragma once

#include "common/size_definitions.hpp"

namespace Game
{
	struct command_buffer
	{
		uint8* commands		 = nullptr;
		uint16 command_count = 0;
		uint8  pad[48];
	};
}
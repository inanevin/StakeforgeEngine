// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "resources/common_resources.hpp"

namespace vekt
{
	class font_manager;
	struct font;
}

namespace SFG
{
	struct font_raw;

	class font
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_font;

		void create_from_raw(const font_raw& raw, vekt::font_manager& fm);

	private:
		vekt::font* _font = nullptr;
	};

}

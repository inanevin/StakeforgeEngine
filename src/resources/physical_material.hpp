// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "resources/common_resources.hpp"

namespace SFG
{
	struct physical_material_raw;

	class physical_material
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_physical_material;

		void create_from_raw(const physical_material_raw& raw);

	private:
	};

}

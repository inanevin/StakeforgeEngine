// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/shader_description.hpp"
#include "resources/common_resources.hpp"
namespace SFG
{
	struct shader_raw;

	class shader
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_shader;

		enum flags
		{
			is_skinned = 1 << 0,
			hw_exists  = 1 << 1,
		};

		~shader();

		void   create_from_raw(shader_raw& raw, bool use_embedded_layout, gfx_id layout);
		void   destroy();
		gfx_id get_hw() const;

		inline const bitmask<uint8>& get_flags() const
		{
			return _flags;
		}

	private:
		gfx_id		   _hw	  = 0;
		bitmask<uint8> _flags = 0;
	};

}

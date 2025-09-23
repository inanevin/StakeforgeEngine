// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/shader_description.hpp"
#include "resources/common_resources.hpp"

#include <limits>

#undef min
#undef max

namespace SFG
{
	struct shader_desc;

	struct shader_meta
	{
		string		   source	  = "";
		shader_desc	   desc		  = {};
		uint8		   is_skinned = 0;
		vector<string> defines;
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const shader_meta& s);
	void from_json(const nlohmann::json& j, shader_meta& s);
#endif

	class shader
	{
	public:
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_shader;

		enum flags
		{
			is_skinned = 1 << 0,
		};

		~shader();

#ifdef SFG_TOOLMODE
		bool create_from_file_vertex_pixel(const char* file, bool use_embedded_layout, gfx_id layout);
#endif
		void   destroy();
		gfx_id get_hw() const;

		static void get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block);

		inline const bitmask<uint8>& get_flags() const
		{
			return _flags;
		}

	private:
		gfx_id		   _hw	  = std::numeric_limits<gfx_id>::max();
		bitmask<uint8> _flags = 0;
	};

}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "gfx/common/shader_description.hpp"

namespace SFG
{
	struct shader_desc;

	struct shader_meta
	{
		string		source = "";
		shader_desc desc   = {};
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const shader_meta& s);
	void from_json(const nlohmann::json& j, shader_meta& s);
#endif

	class shader
	{
	public:
		~shader();

#ifdef SFG_TOOLMODE
		bool create_from_file_vertex_pixel(const char* file, bool use_embedded_layout, gfx_id layout);
#endif
		void   destroy();
		gfx_id get_hw() const;

		static void get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block);

	private:
		gfx_id _hw = 0;
	};

}

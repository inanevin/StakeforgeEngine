// Copyright (c) 2025 Inan Evin

#include "shader.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/shader_description.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include "vendor/nhlohmann/json.hpp"
using json = nlohmann::json;
#endif

#include <fstream>

#undef min
#undef max

namespace SFG
{

#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const shader_meta& s)
	{
		j["source"]		= s.source;
		j["desc"]		= s.desc;
		j["defines"]	= s.defines;
		j["is_skinned"] = s.is_skinned;
	}

	void from_json(const nlohmann::json& j, shader_meta& s)
	{
		s.source	 = j.value<string>("source", "");
		s.desc		 = j.value<shader_desc>("desc", {});
		s.defines	 = j.value<vector<string>>("defines", {});
		s.is_skinned = j.value<uint8>("is_skinned", 0);
	}

#endif

	shader::~shader()
	{
		SFG_ASSERT(_hw == std::numeric_limits<gfx_id>::max());
	}

	void shader::destroy()
	{
		gfx_backend::get()->destroy_shader(_hw);
		_hw = std::numeric_limits<gfx_id>::max();
	}

#ifdef SFG_TOOLMODE

	bool shader::create_from_file_vertex_pixel(const char* path, bool use_embedded_layout, gfx_id layout)
	{
		SFG_ASSERT(_hw == std::numeric_limits<gfx_id>::max());

		if (!file_system::exists(path))
		{
			SFG_ERR("File don't exist! {0}", path);
			return false;
		}

		std::ifstream f(path);
		json		  json_data = json::parse(f);
		shader_meta	  meta		= json_data;
		f.close();

		const string shader_text = file_system::read_file_as_string(meta.source.c_str());
		if (shader_text.empty())
			return false;

		_flags = 0;
		_flags.set(shader::flags::is_skinned, meta.is_skinned);

		gfx_backend* backend = gfx_backend::get();
		meta.desc.debug_name = meta.source.c_str();

		meta.desc.blobs = {
			{.stage = shader_stage::vertex},
			{.stage = shader_stage::fragment},
		};

		if (use_embedded_layout)
			meta.desc.flags.set(shader_flags::shf_use_embedded_layout);
		else
			meta.desc.layout = layout;

		span<uint8> layout_data	   = {};
		const bool	compile_layout = meta.desc.flags.is_set(shader_flags::shf_use_embedded_layout);

		const string folder_path = file_system::get_directory_of_file(meta.source.c_str());
		if (!backend->compile_shader_vertex_pixel(shader_text, meta.defines, folder_path.c_str(), meta.desc.vertex_entry.c_str(), meta.desc.pixel_entry.c_str(), meta.desc.blobs[0].data, meta.desc.blobs[1].data, compile_layout, meta.desc.layout_data))
			return false;

		_hw = backend->create_shader(meta.desc);

		for (shader_blob& b : meta.desc.blobs)
			delete[] b.data.data;

		if (compile_layout)
			delete[] meta.desc.layout_data.data;

		meta.desc.layout_data = {};
		meta.desc.blobs		  = {};

		return true;
	}

#endif

	void shader::get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block)
	{
		size_t block_start = text.find(block_ident);
		if (block_start == std::string::npos)
			return;

		block_start += block_ident.length();

		size_t block_end = text.find(end_ident, block_start);
		if (block_end == std::string::npos)
			return;

		out_block = text.substr(block_start, block_end - block_start);
	}

	gfx_id shader::get_hw() const
	{
		return _hw;
	}

} // namespace Lina

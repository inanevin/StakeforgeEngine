// Copyright (c) 2025 Inan Evin

#include "shader.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "io/file_system.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"

namespace Game
{
	void shader::destroy()
	{
		gfx_backend::get()->destroy_shader(_hw);
	}

	bool shader::create_from_file_vertex_pixel(const char* path, const shader_desc& desc)
	{
		const string shader_text = file_system::read_file_as_string(path);
		if (shader_text.empty())
			return false;

		gfx_backend* backend = gfx_backend::get();

		shader_desc used_desc = desc;
		used_desc.blobs		  = {
			  {.stage = shader_stage::vertex},
			  {.stage = shader_stage::fragment},
		  };

		const bool compile_layout = desc.flags.is_set(shader_flags::shf_use_embedded_layout);

		if (!backend->compile_shader_vertex_pixel(shader_text, desc.vertex_entry, desc.pixel_entry, used_desc.blobs[0].data, used_desc.blobs[1].data, compile_layout, used_desc.layout_data))
			return false;

		_hw = backend->create_shader(used_desc);

		for (shader_blob& b : used_desc.blobs)
			delete[] b.data.data;

		if (compile_layout)
			delete[] used_desc.layout_data.data;

		return true;
	}

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
} // namespace Lina

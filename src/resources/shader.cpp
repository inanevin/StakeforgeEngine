// Copyright (c) 2025 Inan Evin

#include "shader.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "io/file_system.hpp"
#include "gfx/backend/backend.hpp"

namespace Game
{
	void shader::destroy()
	{
		gfx_backend::get()->destroy_shader(_hw);
	}

	bool shader::create_from_file_vertex_pixel(const char* path)
	{
		const string shader_text = file_system::read_file_as_string(path);
		if (shader_text.empty())
			return false;

		gfx_backend* backend = gfx_backend::get();

		_desc.blobs = {
			{.stage = shader_stage::vertex},
			{.stage = shader_stage::fragment},
		};

		span<uint8> layout_data	   = {};
		const bool	compile_layout = _desc.flags.is_set(shader_flags::shf_use_embedded_layout);

		if (!backend->compile_shader_vertex_pixel(shader_text, _desc.vertex_entry, _desc.pixel_entry, _desc.blobs[0].data, _desc.blobs[1].data, compile_layout, _desc.layout_data))
			return false;

		_hw = backend->create_shader(_desc);

		for (shader_blob& b : _desc.blobs)
			delete[] b.data.data;

		if (compile_layout)
			delete[] _desc.layout_data.data;

		_desc.layout_data = {};
		_desc.blobs		  = {};

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

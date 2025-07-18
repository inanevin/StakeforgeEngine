// Copyright (c) 2025 Inan Evin

#include "shader.hpp"
#include "common/linagx_incl.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "io/file_system.hpp"
#include <LinaGX/Utility/SPIRVUtility.hpp>
#include <LinaGX/Common/CommonData.hpp>

namespace Game
{
	void shader::destroy()
	{
		LinaGX::instance()->DestroyShader(_hw);
	}

	bool shader::create_from_file_default(const char* path, const LinaGX::ShaderDesc& desc)
	{
		const string shader_text = file_system::read_file_as_string(path);

		if (shader_text.empty()) return false;

		const string running_dir = file_system::get_running_directory();

		LinaGX::ShaderCompileData vertex_data = {.stage = LinaGX::ShaderStage::Vertex, .includePath = running_dir};
		LinaGX::ShaderCompileData frag_data	  = {.stage = LinaGX::ShaderStage::Fragment, .includePath = running_dir};
		get_shader_block(shader_text, "#vertex", "#end", vertex_data.text);
		get_shader_block(shader_text, "#frag", "#end", frag_data.text);

		LinaGX::ShaderLayout				  layout;
		LINAGX_VEC<LinaGX::ShaderCompileData> compile_data;
		compile_data.push_back(vertex_data);
		compile_data.push_back(frag_data);
		if (!LinaGX::Instance::CompileShader(compile_data, layout)) { return false; }

		LinaGX::ShaderDesc used_desc = desc;
		used_desc.stages			 = compile_data;
		used_desc.layout			 = layout;
		used_desc.debugName			 = path;
		_hw							 = LinaGX::instance()->CreateShader(used_desc);
		return true;
	}

	void shader::get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block)
	{
		size_t block_start = text.find(block_ident);
		if (block_start == std::string::npos) return;

		block_start += block_ident.length(); // Skip past the identifier itself

		size_t block_end = text.find(end_ident, block_start);
		if (block_end == std::string::npos) return;

		out_block = text.substr(block_start, block_end - block_start);
	}
} // namespace Lina

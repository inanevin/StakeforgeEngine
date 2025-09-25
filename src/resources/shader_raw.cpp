// Copyright (c) 2025 Inan Evin

#include "shader_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include "project/engine_data.hpp"
#include "gfx/backend/backend.hpp"
#include "vendor/nhlohmann/json.hpp"
#include <fstream>
using json = nlohmann::json;
#endif

namespace SFG
{

	shader_raw::~shader_raw()
	{
		for (const shader_blob& b : desc.blobs)
			delete[] b.data.data;

		if (desc.layout_data.size != 0)
			delete[] desc.layout_data.data;
	}

	void shader_raw::serialize(ostream& stream, const shader_desc& desc) const
	{
		stream << desc.vertex_entry;
		stream << desc.pixel_entry;
		stream << desc.compute_entry;
		stream << desc.flags.value();
		stream << static_cast<uint32>(desc.layout_data.size);

		if (desc.layout_data.size != 0)
			stream.write_raw(desc.layout_data.data, desc.layout_data.size);

		const uint16 blobs_count = static_cast<uint16>(desc.blobs.size());
		stream << blobs_count;

		for (const shader_blob& b : desc.blobs)
		{
			stream << b.stage;
			stream << static_cast<uint32>(b.data.size);
			stream.write_raw(b.data.data, b.data.size);
		}

		const uint16 att_count = static_cast<uint16>(desc.attachments.size());
		stream << att_count;

		for (const shader_color_attachment& att : desc.attachments)
		{
			stream << att.format;
			stream << att.blend_attachment.blend_enabled;
			stream << att.blend_attachment.src_alpha_blend_factor;
			stream << att.blend_attachment.dst_alpha_blend_factor;
			stream << att.blend_attachment.color_blend_op;
			stream << att.blend_attachment.src_alpha_blend_factor;
			stream << att.blend_attachment.dst_alpha_blend_factor;
			stream << att.blend_attachment.alpha_blend_op;
			stream << att.blend_attachment.color_comp_flags.value();
		}

		// depth stencil
		{
			stream << desc.depth_stencil_desc.attachment_format;
			stream << desc.depth_stencil_desc.depth_compare;
			stream << desc.depth_stencil_desc.back_stencil_state.compare_op;
			stream << desc.depth_stencil_desc.back_stencil_state.depth_fail_op;
			stream << desc.depth_stencil_desc.back_stencil_state.fail_op;
			stream << desc.depth_stencil_desc.back_stencil_state.pass_op;
			stream << desc.depth_stencil_desc.front_stencil_state.compare_op;
			stream << desc.depth_stencil_desc.front_stencil_state.depth_fail_op;
			stream << desc.depth_stencil_desc.front_stencil_state.fail_op;
			stream << desc.depth_stencil_desc.front_stencil_state.pass_op;
			stream << desc.depth_stencil_desc.stencil_compare_mask;
			stream << desc.depth_stencil_desc.stencil_write_mask;
			stream << desc.depth_stencil_desc.flags.value();
			stream << desc.depth_bias_clamp;
			stream << desc.depth_bias_constant;
			stream << desc.depth_bias_slope;
		}

		const uint16 inp_count = static_cast<uint16>(desc.inputs.size());
		stream << inp_count;

		for (const vertex_input& inp : desc.inputs)
		{
			stream << inp.name;
			stream << inp.location;
			stream << inp.index;
			stream << static_cast<uint32>(inp.offset);
			stream << static_cast<uint32>(inp.size);
			stream << inp.format;
		}

		stream << desc.blend_logic_op;
		stream << desc.topo;
		stream << desc.cull;
		stream << desc.front;
		stream << desc.poly_mode;
		stream << desc.samples;
	}

	void shader_raw::deserialize(istream& stream, shader_desc& desc)
	{
		uint8  sh_flags	   = 0;
		uint32 layout_size = 0;

		stream >> desc.vertex_entry;
		stream >> desc.pixel_entry;
		stream >> desc.compute_entry;
		stream >> sh_flags;
		stream >> layout_size;
		desc.flags			  = sh_flags;
		desc.layout_data.size = static_cast<size_t>(layout_size);

		if (desc.layout_data.size != 0)
		{
			desc.layout_data.data = new uint8[desc.layout_data.size];
			stream.read_to_raw(desc.layout_data.data, desc.layout_data.size);
		}

		uint16 blobs_count = 0;
		stream >> blobs_count;

		desc.blobs.resize(static_cast<size_t>(blobs_count));

		for (uint16 i = 0; i < blobs_count; i++)
		{
			shader_blob& b = desc.blobs[i];
			stream >> b.stage;
			uint32 size = 0;
			stream >> size;
			b.data.data = new uint8[size];
			b.data.size = static_cast<size_t>(size);
			stream.read_to_raw(b.data.data, b.data.size);
		}

		uint16 att_count = 0;
		stream >> att_count;
		desc.attachments.resize(att_count);
		for (uint16 i = 0; i < att_count; i++)
		{
			shader_color_attachment& att   = desc.attachments[i];
			uint8					 flags = 0;
			stream >> att.format;
			stream >> att.blend_attachment.blend_enabled;
			stream >> att.blend_attachment.src_alpha_blend_factor;
			stream >> att.blend_attachment.dst_alpha_blend_factor;
			stream >> att.blend_attachment.color_blend_op;
			stream >> att.blend_attachment.src_alpha_blend_factor;
			stream >> att.blend_attachment.dst_alpha_blend_factor;
			stream >> att.blend_attachment.alpha_blend_op;
			stream >> flags;
			att.blend_attachment.color_comp_flags = flags;
		}

		// depth stencil
		{
			uint8 flags = 0;
			stream >> desc.depth_stencil_desc.attachment_format;
			stream >> desc.depth_stencil_desc.depth_compare;
			stream >> desc.depth_stencil_desc.back_stencil_state.compare_op;
			stream >> desc.depth_stencil_desc.back_stencil_state.depth_fail_op;
			stream >> desc.depth_stencil_desc.back_stencil_state.fail_op;
			stream >> desc.depth_stencil_desc.back_stencil_state.pass_op;
			stream >> desc.depth_stencil_desc.front_stencil_state.compare_op;
			stream >> desc.depth_stencil_desc.front_stencil_state.depth_fail_op;
			stream >> desc.depth_stencil_desc.front_stencil_state.fail_op;
			stream >> desc.depth_stencil_desc.front_stencil_state.pass_op;
			stream >> desc.depth_stencil_desc.stencil_compare_mask;
			stream >> desc.depth_stencil_desc.stencil_write_mask;
			stream >> flags;
			desc.depth_stencil_desc.flags = flags;
			stream >> desc.depth_bias_clamp;
			stream >> desc.depth_bias_constant;
			stream >> desc.depth_bias_slope;
		}

		uint16 inp_count = 0;
		stream >> inp_count;
		desc.inputs.resize(inp_count);
		for (uint16 i = 0; i < inp_count; i++)
		{
			vertex_input& inp	 = desc.inputs[i];
			uint32		  offset = 0;
			uint32		  size	 = 0;

			stream >> inp.name;
			stream >> inp.location;
			stream >> inp.index;
			stream >> offset;
			stream >> size;
			stream >> inp.format;

			inp.offset = static_cast<size_t>(offset);
			inp.size   = static_cast<size_t>(size);
		}

		stream >> desc.blend_logic_op;
		stream >> desc.topo;
		stream >> desc.cull;
		stream >> desc.front;
		stream >> desc.poly_mode;
		stream >> desc.samples;
	}

	void shader_raw::serialize(ostream& stream) const
	{
		stream << name;
		stream << defines;
		stream << is_skinned;
		serialize(stream, desc);
	}

	void shader_raw::deserialize(istream& stream)
	{
		stream >> name;
		stream >> defines;
		stream >> is_skinned;
		deserialize(stream, desc);
	}

#ifdef SFG_TOOLMODE

	bool shader_raw::cook_from_file(const char* path)
	{
		if (!file_system::exists(path))
		{
			SFG_ERR("File don't exist! {0}", path);
			return false;
		}
		try
		{
			std::ifstream f(path);
			json		  json_data = json::parse(f);
			f.close();

			name	   = json_data.value<string>("source", "");
			desc	   = json_data.value<shader_desc>("desc", {});
			defines	   = json_data.value<vector<string>>("defines", {});
			is_skinned = json_data.value<uint8>("is_skinned", 0);

			const string source = engine_data::get().get_working_dir() + name;
			if (!file_system::exists(source.c_str()))
			{
				SFG_ERR("File don't exist! {0}", path);
				return false;
			}

			const string shader_text = file_system::read_file_as_string(source.c_str());
			if (shader_text.empty())
				return false;

			desc.debug_name = name.c_str();
			desc.blobs		= {
				 {.stage = shader_stage::vertex},
				 {.stage = shader_stage::fragment},
			 };

			span<uint8> layout_data	   = {};
			const bool	compile_layout = desc.flags.is_set(shader_flags::shf_use_embedded_layout);

			gfx_backend* backend	 = gfx_backend::get();
			const string folder_path = file_system::get_directory_of_file(source.c_str());
			if (!backend->compile_shader_vertex_pixel(shader_text, defines, folder_path.c_str(), desc.vertex_entry.c_str(), desc.pixel_entry.c_str(), desc.blobs[0].data, desc.blobs[1].data, compile_layout, desc.layout_data))
				return false;
		}
		catch (std::exception e)
		{
			SFG_ERR("Failed loading shader: {0}", e.what());
			return false;
		}

		SFG_INFO("Created shader from file: {0}", path);
		return true;
	}
#endif
}

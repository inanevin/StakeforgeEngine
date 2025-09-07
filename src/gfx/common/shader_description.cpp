// Copyright (c) 2025 Inan Evin

#include "shader_description.hpp"
#include "io/assert.hpp"

#include "vendor/nhlohmann/json.hpp"
using json = nlohmann::json;

namespace SFG
{
#ifdef SFG_TOOLMODE
	void to_json(nlohmann::json& j, const vertex_input& s)
	{
		j["name"]	  = s.name;
		j["location"] = s.location;
		j["index"]	  = s.index;
		j["offset"]	  = s.offset;
		j["size"]	  = s.size;
		j["format"]	  = s.format;
	}

	void from_json(const nlohmann::json& j, vertex_input& s)
	{
		s.name	   = j.value<string>("name", "");
		s.location = j.value<uint8>("location", 0);
		s.index	   = j.value<uint8>("index", 0);
		s.offset   = j.value<uint8>("offset", 0);
		s.size	   = j.value<uint32>("size", 0);
		s.format   = j.value<format>("format", format::undefined);
	}

	void to_json(nlohmann::json& j, const shader_desc& s)
	{
		j["pixel_entry"]	   = s.pixel_entry;
		j["vertex_entry"]	   = s.vertex_entry;
		j["compute_entry"]	   = s.compute_entry;
		j["cull"]			   = s.cull;
		j["front"]			   = s.front;
		j["vertex_inputs"]	   = s.inputs;
		j["color_attachments"] = s.attachments;
		j["samples"]		   = s.samples;
		j["depth_stencil"]	   = s.depth_stencil_desc;
	}

	void from_json(const nlohmann::json& j, shader_desc& s)
	{
		s.pixel_entry		 = j.value<string>("pixel_entry", "");
		s.vertex_entry		 = j.value<string>("vertex_entry", "");
		s.compute_entry		 = j.value<string>("compute_entry", "");
		s.cull				 = j.value<cull_mode>("cull", cull_mode::back);
		s.front				 = j.value<front_face>("front", front_face::ccw);
		s.inputs			 = j.value<vector<vertex_input>>("vertex_inputs", {});
		s.attachments		 = j.value<vector<shader_color_attachment>>("color_attachments", {});
		s.samples			 = j.value<uint32>("samples", 1);
		s.depth_stencil_desc = j.value<shader_depth_stencil_desc>("depth_stencil", {});
	}

	void to_json(nlohmann::json& j, const cull_mode& c)
	{
		switch (c)
		{
		case cull_mode::none:
			j = "none";
			break;
		case cull_mode::front:
			j = "front";
			break;
		case cull_mode::back:
			j = "back";
			break;
		}
	}

	void from_json(const nlohmann::json& j, cull_mode& c)
	{
		const string& str = j.get<string>();
		if (str.compare("none") == 0)
		{
			c = cull_mode::none;
			return;
		}

		if (str.compare("back") == 0)
		{
			c = cull_mode::back;
			return;
		}
		if (str.compare("front") == 0)
		{
			c = cull_mode::front;
			return;
		}

		SFG_ASSERT(false);
	}

	void to_json(nlohmann::json& j, const front_face& f)
	{
		switch (f)
		{
		case front_face::ccw:
			j = "ccw";
			break;
		case front_face::cw:
			j = "cw";
			break;
		}
	}

	void from_json(const nlohmann::json& j, front_face& f)
	{
		const string& str = j.get<string>();
		if (str.compare("cw") == 0)
		{
			f = front_face::cw;
			return;
		}

		if (str.compare("ccw") == 0)
		{
			f = front_face::ccw;
			return;
		}

		SFG_ASSERT(false);
	}

	void to_json(nlohmann::json& j, const blend_factor& f)
	{
		switch (f)
		{
		case blend_factor::zero:
			j = "zero";
			break;
		case blend_factor::one:
			j = "one";
			break;
		case blend_factor::src_color:
			j = "src_color";
			break;
		case blend_factor::one_minus_src_color:
			j = "one_minus_src_color";
			break;
		case blend_factor::dst_color:
			j = "dst_color";
			break;
		case blend_factor::one_minus_dst_color:
			j = "one_minus_dst_color";
			break;
		case blend_factor::src_alpha:
			j = "src_alpha";
			break;
		case blend_factor::one_minus_src_alpha:
			j = "one_minus_src_alpha";
			break;
		case blend_factor::dst_alpha:
			j = "dst_alpha";
			break;
		case blend_factor::one_minus_dst_alpha:
			j = "one_minus_dst_alpha";
			break;
		}
	}

	void from_json(const nlohmann::json& j, blend_factor& f)
	{
		const string& str = j.get<string>();

		if (str.compare("zero") == 0)
		{
			f = blend_factor::zero;
			return;
		}

		if (str.compare("one") == 0)
		{
			f = blend_factor::one;
			return;
		}

		if (str.compare("src_color") == 0)
		{
			f = blend_factor::src_color;
			return;
		}

		if (str.compare("one_minus_src_color") == 0)
		{
			f = blend_factor::one_minus_src_color;
			return;
		}

		if (str.compare("dst_color") == 0)
		{
			f = blend_factor::dst_color;
			return;
		}

		if (str.compare("one_minus_dst_color") == 0)
		{
			f = blend_factor::one_minus_dst_color;
			return;
		}

		if (str.compare("src_alpha") == 0)
		{
			f = blend_factor::src_alpha;
			return;
		}

		if (str.compare("one_minus_src_alpha") == 0)
		{
			f = blend_factor::one_minus_src_alpha;
			return;
		}

		if (str.compare("dst_alpha") == 0)
		{
			f = blend_factor::dst_alpha;
			return;
		}

		if (str.compare("one_minus_dst_alpha") == 0)
		{
			f = blend_factor::one_minus_dst_alpha;
			return;
		}

		SFG_ASSERT(false);
	}

	void to_json(nlohmann::json& j, const blend_op& op)
	{
		switch (op)
		{
		case blend_op::add:
			j = "add";
			break;
		case blend_op::subtract:
			j = "subtract";
			break;
		case blend_op::reverse_subtract:
			j = "reverse_subtract";
			break;
		case blend_op::min:
			j = "min";
			break;
		case blend_op::max:
			j = "max";
			break;
		}
	}

	void from_json(const nlohmann::json& j, blend_op& op)
	{
		const string& str = j.get<string>();

		if (str.compare("add") == 0)
		{
			op = blend_op::add;
			return;
		}

		if (str.compare("subtract") == 0)
		{
			op = blend_op::subtract;
			return;
		}

		if (str.compare("reverse_subtract") == 0)
		{
			op = blend_op::reverse_subtract;
			return;
		}

		if (str.compare("min") == 0)
		{
			op = blend_op::min;
			return;
		}

		if (str.compare("max") == 0)
		{
			op = blend_op::max;
			return;
		}

		SFG_ASSERT(false);
	}

	// --- stencil_op ---
	void to_json(nlohmann::json& j, const stencil_op& op)
	{
		switch (op)
		{
		case stencil_op::keep:
			j = "keep";
			break;
		case stencil_op::zero:
			j = "zero";
			break;
		case stencil_op::replace:
			j = "replace";
			break;
		case stencil_op::increment_clamp:
			j = "increment_clamp";
			break;
		case stencil_op::decrement_clamp:
			j = "decrement_clamp";
			break;
		case stencil_op::invert:
			j = "invert";
			break;
		case stencil_op::increment_wrap:
			j = "increment_wrap";
			break;
		case stencil_op::decrement_wrap:
			j = "decrement_wrap";
			break;
		}
	}

	void from_json(const nlohmann::json& j, stencil_op& op)
	{
		const std::string& str = j.get<std::string>();

		if (str.compare("keep") == 0)
		{
			op = stencil_op::keep;
			return;
		}
		if (str.compare("zero") == 0)
		{
			op = stencil_op::zero;
			return;
		}
		if (str.compare("replace") == 0)
		{
			op = stencil_op::replace;
			return;
		}
		if (str.compare("increment_clamp") == 0)
		{
			op = stencil_op::increment_clamp;
			return;
		}
		if (str.compare("decrement_clamp") == 0)
		{
			op = stencil_op::decrement_clamp;
			return;
		}
		if (str.compare("invert") == 0)
		{
			op = stencil_op::invert;
			return;
		}
		if (str.compare("increment_wrap") == 0)
		{
			op = stencil_op::increment_wrap;
			return;
		}
		if (str.compare("decrement_wrap") == 0)
		{
			op = stencil_op::decrement_wrap;
			return;
		}

		SFG_ASSERT(false);
	}

	// --- load_op ---
	void to_json(nlohmann::json& j, const load_op& op)
	{
		switch (op)
		{
		case load_op::load:
			j = "load";
			break;
		case load_op::clear:
			j = "clear";
			break;
		case load_op::dont_care:
			j = "dont_care";
			break;
		case load_op::none:
			j = "none";
			break;
		}
	}

	void from_json(const nlohmann::json& j, load_op& op)
	{
		const std::string& str = j.get<std::string>();

		if (str.compare("load") == 0)
		{
			op = load_op::load;
			return;
		}
		if (str.compare("clear") == 0)
		{
			op = load_op::clear;
			return;
		}
		if (str.compare("dont_care") == 0)
		{
			op = load_op::dont_care;
			return;
		}
		if (str.compare("none") == 0)
		{
			op = load_op::none;
			return;
		}

		SFG_ASSERT(false);
	}

	// --- compare_op ---
	void to_json(nlohmann::json& j, const compare_op& op)
	{
		switch (op)
		{
		case compare_op::never:
			j = "never";
			break;
		case compare_op::less:
			j = "less";
			break;
		case compare_op::equal:
			j = "equal";
			break;
		case compare_op::lequal:
			j = "lequal";
			break;
		case compare_op::greater:
			j = "greater";
			break;
		case compare_op::nequal:
			j = "nequal";
			break;
		case compare_op::gequal:
			j = "gequal";
			break;
		case compare_op::always:
			j = "always";
			break;
		}
	}

	void from_json(const nlohmann::json& j, compare_op& op)
	{
		const std::string& str = j.get<std::string>();

		if (str.compare("never") == 0)
		{
			op = compare_op::never;
			return;
		}
		if (str.compare("less") == 0)
		{
			op = compare_op::less;
			return;
		}
		if (str.compare("equal") == 0)
		{
			op = compare_op::equal;
			return;
		}
		if (str.compare("lequal") == 0)
		{
			op = compare_op::lequal;
			return;
		}
		if (str.compare("greater") == 0)
		{
			op = compare_op::greater;
			return;
		}
		if (str.compare("nequal") == 0)
		{
			op = compare_op::nequal;
			return;
		}
		if (str.compare("gequal") == 0)
		{
			op = compare_op::gequal;
			return;
		}
		if (str.compare("always") == 0)
		{
			op = compare_op::always;
			return;
		}

		SFG_ASSERT(false);
	}

	// --- store_op ---
	void to_json(nlohmann::json& j, const store_op& op)
	{
		switch (op)
		{
		case store_op::store:
			j = "store";
			break;
		case store_op::dont_care:
			j = "dont_care";
			break;
		case store_op::none:
			j = "none";
			break;
		}
	}

	void from_json(const nlohmann::json& j, store_op& op)
	{
		const std::string& str = j.get<std::string>();

		if (str.compare("store") == 0)
		{
			op = store_op::store;
			return;
		}
		if (str.compare("dont_care") == 0)
		{
			op = store_op::dont_care;
			return;
		}
		if (str.compare("none") == 0)
		{
			op = store_op::none;
			return;
		}

		SFG_ASSERT(false);
	}

	void to_json(nlohmann::json& j, const color_blend_attachment& att)
	{
		j["blend_enabled"]			= att.blend_enabled;
		j["src_color_blend_factor"] = att.src_color_blend_factor;
		j["dst_color_blend_factor"] = att.dst_color_blend_factor;
		j["color_blend_op"]			= att.color_blend_op;
		j["src_alpha_blend_factor"] = att.src_alpha_blend_factor;
		j["dst_alpha_blend_factor"] = att.dst_alpha_blend_factor;
		j["alpha_blend_op"]			= att.alpha_blend_op;
		j["color_comp_flags"]		= att.color_comp_flags.value();
	}

	void from_json(const nlohmann::json& j, color_blend_attachment& att)
	{
		att.blend_enabled		   = j.value("blend_enabled", false);
		att.src_color_blend_factor = j.value("src_color_blend_factor", att.src_color_blend_factor);
		att.dst_color_blend_factor = j.value("dst_color_blend_factor", att.dst_color_blend_factor);
		att.color_blend_op		   = j.value("color_blend_op", att.color_blend_op);
		att.src_alpha_blend_factor = j.value("src_alpha_blend_factor", att.src_alpha_blend_factor);
		att.dst_alpha_blend_factor = j.value("dst_alpha_blend_factor", att.dst_alpha_blend_factor);
		att.alpha_blend_op		   = j.value("alpha_blend_op", att.alpha_blend_op);

		const uint8 color	 = j.value<uint8>("color_comp_flags", 0);
		att.color_comp_flags = color;
	}

	void to_json(nlohmann::json& j, const shader_color_attachment& att)
	{
		j["format"]			  = att.format;
		j["blend_attachment"] = att.blend_attachment;
	}

	void from_json(const nlohmann::json& j, shader_color_attachment& att)
	{
		att.format			 = j.value<format>("format", format::undefined);
		att.blend_attachment = j.value<color_blend_attachment>("blend_attachment", {});
	}

	void to_json(nlohmann::json& j, const stencil_state& ss)
	{
		j["compare_op"]	   = ss.compare_op;
		j["depth_fail_op"] = ss.depth_fail_op;
		j["fail_op"]	   = ss.fail_op;
		j["pass_op"]	   = ss.pass_op;
	}

	void from_json(const nlohmann::json& j, stencil_state& ss)
	{
		ss.compare_op	 = j.value("compare_op", compare_op::always);
		ss.depth_fail_op = j.value("depth_fail_op", stencil_op::keep);
		ss.fail_op		 = j.value("fail_op", stencil_op::keep);
		ss.pass_op		 = j.value("pass_op", stencil_op::keep);
	}

	void to_json(nlohmann::json& j, const shader_depth_stencil_desc& att)
	{
		j["attachment_format"]	  = att.attachment_format;
		j["depth_compare"]		  = att.depth_compare;
		j["back_stencil_state"]	  = att.back_stencil_state;
		j["front_stencil_state"]  = att.front_stencil_state;
		j["stencil_compare_mask"] = att.stencil_compare_mask;
		j["stencil_write_mask"]	  = att.stencil_write_mask;
		j["flags"]				  = att.flags.value();
	}

	void from_json(const nlohmann::json& j, shader_depth_stencil_desc& att)
	{
		att.attachment_format	 = j.value("attachment_format", format::d32_sfloat);
		att.depth_compare		 = j.value("depth_compare", compare_op::lequal);
		att.back_stencil_state	 = j.value("back_stencil_state", stencil_state{});
		att.front_stencil_state	 = j.value("front_stencil_state", stencil_state{});
		att.stencil_compare_mask = j.value("stencil_compare_mask", static_cast<uint32>(0xFF));
		att.stencil_write_mask	 = j.value("stencil_write_mask", static_cast<uint32>(0xFF));
		att.flags				 = j.value<uint8>("flags", 0);
	}
#endif
}
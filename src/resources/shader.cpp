// Copyright (c) 2025 Inan Evin

#include "shader.hpp"
#include "shader_raw.hpp"
#include "data/string.hpp"
#include "data/vector.hpp"
#include "gfx/backend/backend.hpp"

namespace SFG
{
	shader::~shader()
	{
		SFG_ASSERT(!_flags.is_set(shader::flags::hw_exists));
	}

	void shader::create_from_raw(shader_raw& raw, bool use_embedded_layout, gfx_id layout)
	{
		SFG_ASSERT(!_flags.is_set(shader::flags::hw_exists));
		gfx_backend* backend = gfx_backend::get();

		if (use_embedded_layout)
			raw.desc.flags.set(shader_flags::shf_use_embedded_layout);
		else
			raw.desc.layout = layout;

		_hw = backend->create_shader(raw.desc);
		_flags.set(shader::flags::hw_exists);
		_flags.set(shader::flags::is_skinned, raw.is_skinned);
	}

	void shader::destroy()
	{
		SFG_ASSERT(_flags.is_set(shader::flags::hw_exists));
		gfx_backend::get()->destroy_shader(_hw);
		_hw = 0;
		_flags.remove(shader::flags::hw_exists);
	}

	gfx_id shader::get_hw() const
	{
		return _hw;
	}

} // namespace Lina

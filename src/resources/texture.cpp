// Copyright (c) 2025 Inan Evin

#include "texture.hpp"
#include "texture_raw.hpp"
#include "memory/memory.hpp"
#include "memory/memory_tracer.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"

namespace SFG
{
	texture::~texture()
	{
		SFG_ASSERT(_cpu_buffers.empty(), "");
		SFG_ASSERT(!_flags.is_set(texture::flags::hw_exists));
	}

	void texture::create_from_raw(const texture_raw& raw)
	{
		_cpu_buffers = raw.buffers;

		SFG_ASSERT(_cpu_buffers.empty());

		gfx_backend* backend = gfx_backend::get();
		_hw					 = backend->create_texture({
							 .texture_format = static_cast<format>(raw.texture_format),
							 .size			 = vector2ui16(get_width(), get_height()),
							 .flags			 = texture_flags::tf_is_2d | texture_flags::tf_sampled,
							 .views			 = {{}},
							 .mip_levels	 = static_cast<uint8>(_cpu_buffers.size()),
							 .array_length	 = 1,
							 .samples		 = 1,
							 .debug_name	 = raw.name.c_str(),

		 });

		_flags.set(texture::flags::hw_exists);

		create_intermediate();
	}

	void texture::destroy_cpu()
	{
		for (texture_buffer& buf : _cpu_buffers)
		{
			PUSH_DEALLOCATION_SZ(buf.size.x * buf.size.y * buf.bpp);
			SFG_FREE(buf.pixels);
		}
		_cpu_buffers.clear();

		if (_flags.is_set(texture::flags::intermediate_exists))
			destroy_intermediate();
	}

	void texture::destroy()
	{
		destroy_cpu();

		SFG_ASSERT(_flags.is_set(texture::flags::hw_exists));
		_flags.remove(texture::flags::hw_exists);
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_texture(_hw);
	}

	uint8 texture::get_bpp() const
	{
		SFG_ASSERT(!_cpu_buffers.empty());
		return _cpu_buffers[0].bpp;
	}

	uint16 texture::get_width() const
	{
		SFG_ASSERT(!_cpu_buffers.empty());
		return _cpu_buffers[0].size.x;
	}

	uint16 texture::get_height() const
	{
		SFG_ASSERT(!_cpu_buffers.empty());
		return _cpu_buffers[0].size.y;
	}

	gfx_id texture::get_hw() const
	{
		SFG_ASSERT(_flags.is_set(texture::flags::hw_exists));
		return _hw;
	}

	void texture::create_intermediate()
	{
		SFG_ASSERT(!_flags.is_set(texture::flags::intermediate_exists));
		gfx_backend* backend = gfx_backend::get();

		uint32 total_size = 0;
		for (const texture_buffer& buf : _cpu_buffers)
		{
			total_size += backend->get_texture_size(buf.size.x, buf.size.y, buf.bpp);
		}

		const uint32 intermediate_size = backend->align_texture_size(total_size);

		_intermediate = backend->create_resource({
			.size		= intermediate_size,
			.flags		= resource_flags::rf_cpu_visible,
			.debug_name = "intermediate_texture_res",
		});

		_flags.set(texture::flags::intermediate_exists);
	}

	void texture::destroy_intermediate()
	{
		SFG_ASSERT(_flags.is_set(texture::flags::intermediate_exists));
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_resource(_intermediate);
		_flags.set(texture::flags::intermediate_exists, false);
	}

}
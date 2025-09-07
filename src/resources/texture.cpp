// Copyright (c) 2025 Inan Evin
#include "texture.hpp"
#include "memory/memory.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"
#include "gfx/util/image_util.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const texture_meta& s)
	{
		j["source"]	  = s.source;
		j["format"]	  = static_cast<format>(s.fmt);
		j["gen_mips"] = s.gen_mips;
	}

	void from_json(const nlohmann::json& j, texture_meta& s)
	{
		s.fmt	   = static_cast<uint8>(j.value<format>("format", format::undefined));
		s.source   = j.value<string>("source", "");
		s.gen_mips = j.value<uint8>("gen_mips", 0);

		SFG_ASSERT(file_system::exists(s.source.c_str()));
		SFG_ASSERT(static_cast<format>(s.fmt) != format::undefined);
	}
#endif

	texture::~texture()
	{
		SFG_ASSERT(_cpu_count == 0, "");
		SFG_ASSERT(!_flags.is_set(texture::flags::hw_exists));
	}

#ifdef SFG_TOOLMODE

	bool texture::create_from_file(const char* file)
	{
		SFG_ASSERT(!_flags.is_set(texture::flags::hw_exists));

		if (!file_system::exists(file))
		{
			SFG_ERR("File doesn't exist! {0}", file);
			return false;
		}

		std::ifstream f(file);
		json		  json_data = json::parse(f);
		texture_meta  meta		= json_data;
		f.close();

		destroy_cpu();

		const format fmt	  = static_cast<format>(static_cast<format>(meta.fmt));
		const uint8	 channels = format_get_channels(fmt);
		const uint8	 bpp	  = format_get_bpp(fmt);

		vector2ui16 size = vector2ui16::zero;
		void*		data = image_util::load_from_file_ch(file, size, channels);

		const texture_buffer b = {
			.pixels = reinterpret_cast<uint8*>(data),
			.size	= size,
			.bpp	= bpp,
		};

		_cpu_count	 = meta.gen_mips ? image_util::calculate_mip_levels(size.x, size.y) : 1;
		_cpu_buffers = new texture_buffer[_cpu_count];

		if (meta.gen_mips == 1)
			populate_mips(channels, format_is_linear(fmt));

		gfx_backend* backend = gfx_backend::get();
		_hw					 = backend->create_texture({
							 .texture_format = fmt,
							 .size			 = vector2ui16(get_width(), get_height()),
							 .views			 = {{}},
							 .mip_levels	 = _cpu_count,
							 .array_length	 = 1,
							 .samples		 = 1,
							 .debug_name	 = meta.source.c_str(),

		 });
		_flags.set(texture::flags::hw_exists | texture::flags::upload_pending);
		create_intermediate();
		return true;
	}
#endif

	void texture::create_from_buffer(uint8* data, const vector2ui16& size, uint8 bpp, uint8 fmt, const char* debug_name)
	{
		destroy_cpu();

		const size_t data_size = static_cast<size_t>(size.x * size.y * bpp);

		_cpu_buffers = new texture_buffer();
		_cpu_count	 = 1;

		texture_buffer& b = _cpu_buffers[0];
		b				  = {
							.pixels = reinterpret_cast<uint8*>(SFG_MALLOC(data_size)),
							.size	= size,
							.bpp	= bpp,
		};

		if (b.pixels)
			SFG_MEMCPY(b.pixels, data, data_size);

		gfx_backend* backend = gfx_backend::get();
		_hw					 = backend->create_texture({
							 .texture_format = static_cast<format>(fmt),
							 .size			 = vector2ui16(get_width(), get_height()),
							 .views			 = {{}},
							 .mip_levels	 = _cpu_count,
							 .array_length	 = 1,
							 .samples		 = 1,
							 .debug_name	 = debug_name,

		 });

		create_intermediate();
	}

	void texture::destroy_cpu()
	{
		for (uint8 i = 0; i < _cpu_count; i++)
			SFG_FREE(_cpu_buffers[i].pixels);

		if (_cpu_count == 1)
			delete _cpu_buffers;
		else
			delete[] _cpu_buffers;
		_cpu_buffers = nullptr;
	}

	void texture::destroy()
	{
		destroy_cpu();

		SFG_ASSERT(_flags.is_set(texture::flags::hw_exists));
		_flags.remove(texture::flags::hw_exists);
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_texture(_hw);

		if (_flags.is_set(texture::flags::intermediate_exists))
			backend->destroy_resource(_intermediate);
	}

	void texture::destroy_intermediate()
	{
		SFG_ASSERT(_flags.is_set(texture::flags::intermediate_exists));
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_resource(_intermediate);
	}

	void texture::populate_mips(uint8 channels, bool is_linear)
	{
		if (_cpu_count == 0)
		{
			SFG_ERR("Can't generate mipmaps as cpu data is empty!");
			return;
		}

		image_util::generate_mips(_cpu_buffers, _cpu_count, image_util::mip_gen_filter::box, channels, is_linear, false);
	}

	uint8 texture::get_bpp() const
	{
		return _cpu_count == 0 ? 0 : _cpu_buffers[0].bpp;
	}

	uint16 texture::get_width() const
	{
		return _cpu_count == 0 ? 0 : _cpu_buffers[0].size.x;
	}

	uint16 texture::get_height() const
	{
		return _cpu_count == 0 ? 0 : _cpu_buffers[0].size.y;
	}

	gfx_id texture::get_hw() const
	{
		SFG_ASSERT(_flags.is_set(texture::flags::hw_exists));
		return _hw;
	}

	void texture::create_intermediate()
	{
		gfx_backend* backend = gfx_backend::get();

		uint32 total_size = 0;
		for (uint32 i = 0; i < _cpu_count++; i++)
		{
			const texture_buffer& buf = _cpu_buffers[i];
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
}
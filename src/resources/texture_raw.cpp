// Copyright (c) 2025 Inan Evin

#include "texture_raw.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"
#include "memory/memory_tracer.hpp"

#ifdef SFG_TOOLMODE

#include "io/file_system.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"
#include "math/vector2ui16.hpp"
#include "math/math.hpp"
#include "project/engine_data.hpp"
#include "gfx/common/format.hpp"
#include "gfx/util/image_util.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;

#endif

namespace SFG
{
	void texture_raw::serialize(ostream& stream) const
	{
		const uint16 count = static_cast<uint16>(buffers.size());
		stream << texture_format;
		stream << count;

		for (const texture_buffer& b : buffers)
		{
			stream << b.size;
			stream << b.bpp;
			stream.write_raw(b.pixels, static_cast<size_t>(b.size.x * b.size.y * b.bpp));
		}
	}

	void texture_raw::deserialize(istream& stream)
	{
		uint16 count = 0;
		stream >> texture_format;
		stream >> count;

		for (uint16 i = 0; i < count; i++)
		{
			texture_buffer& b = buffers[i];
			stream >> b.size;
			stream >> b.bpp;

			const size_t pixels_size = b.size.x * b.size.y * b.bpp;

			SFG_ASSERT(pixels_size != 0);
			b.pixels = reinterpret_cast<uint8*>(SFG_MALLOC(pixels_size));
			PUSH_ALLOCATION_SZ(pixels_size);
			stream.read_to_raw(b.pixels, pixels_size);
		}
	}

#ifdef SFG_TOOLMODE
	bool texture_raw::cook_from_file(const char* file)
	{
		if (!file_system::exists(file))
		{
			SFG_ERR("File doesn't exist! {0}", file);
			return false;
		}

		std::ifstream f(file);
		try
		{
			json json_data = json::parse(f);
			f.close();

			texture_format = static_cast<uint8>(json_data.value<format>("format", format::undefined));
			name		   = json_data.value<string>("source", "");
			gen_mips	   = json_data.value<uint8>("gen_mips", 0);

			const format fmt	   = static_cast<format>(static_cast<format>(texture_format));
			const uint8	 channels  = format_get_channels(fmt);
			const uint8	 bpp	   = format_get_bpp(fmt);
			const bool	 is_linear = format_is_linear(fmt);
			const string source	   = engine_data::get().get_working_dir() + name;
			SFG_ASSERT(file_system::exists(source.c_str()));
			SFG_ASSERT(fmt != format::undefined);

			vector2ui16 size = vector2ui16::zero;
			void*		data = image_util::load_from_file_ch(source.c_str(), size, channels);

			if (data == nullptr)
			{
				SFG_ERR("Failed loading pixel data for texture: {0}", file);
				return false;
			}

			PUSH_ALLOCATION_SZ(size.x * size.y * bpp);

			const texture_buffer b = {
				.pixels = reinterpret_cast<uint8*>(data),
				.size	= size,
				.bpp	= bpp,
			};

			const uint8 count = gen_mips ? math::min(image_util::calculate_mip_levels(size.x, size.y), (uint8)MAX_TEXTURE_MIPS) : 1;

			buffers.resize(count);
			buffers[0] = b;

			if (gen_mips == 1)
				image_util::generate_mips(buffers.data(), count, image_util::mip_gen_filter::box, channels, is_linear, false);
		}
		catch (std::exception e)
		{
			SFG_ERR("Exception: {0}", e.what());
			return false;
		}

		SFG_INFO("Created texture from file: {0}", file);

		return true;
	}

#endif
}

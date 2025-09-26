// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "resources/common_resources.hpp"
#include "data/bitmask.hpp"
#include "memory/chunk_handle.hpp"
#include <vendor/miniaudio/miniaudio.h>

namespace SFG
{
	struct audio_raw;
	class chunk_allocator32;

	class audio
	{
	public:
		enum flags
		{
			is_init		 = 1 << 0,
			is_streaming = 1 << 1,
		};
		static constexpr uint32 TYPE_INDEX = resource_types::resource_type_audio;

		void create_from_raw(const audio_raw& raw, chunk_allocator32& alloc, ma_engine* engine);
		void destroy(chunk_allocator32& alloc);

		inline const ma_decoder& get_decoder() const
		{
			return _decoder;
		}

		inline const bitmask<uint8>& get_flags() const
		{
			return _flags;
		}

	private:
		ma_decoder _decoder{};
		// ma_sound	   _sound{};
		chunk_handle32 _audio_data = {};
		bitmask<uint8> _flags	   = 0;
	};

}

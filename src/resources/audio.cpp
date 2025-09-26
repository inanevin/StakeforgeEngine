// Copyright (c) 2025 Inan Evin

#include "audio.hpp"
#include "audio_raw.hpp"
#include "io/assert.hpp"
#include "memory/chunk_allocator.hpp"
#include <vendor/miniaudio/miniaudio.h>

namespace SFG
{
	void audio::create_from_raw(const audio_raw& raw, chunk_allocator32& alloc, ma_engine* engine)
	{
		if (_flags.is_set(audio::flags::is_init))
			destroy(alloc);

		_flags.set(audio::flags::is_streaming, raw.is_stream);

		if (_flags.is_set(audio::flags::is_streaming))
		{
			_audio_data = alloc.allocate<uint8>(raw.audio_data.size());
			SFG_MEMCPY(alloc.get(_audio_data.head), raw.audio_data.data(), raw.audio_data.size());
		}

		ma_result result;

		char* raw_data = const_cast<char*>(raw.audio_data.data());
		void* data	   = raw.is_stream ? static_cast<void*>(alloc.get(_audio_data.head)) : reinterpret_cast<void*>(raw_data);

		result = ma_decoder_init_memory(data,
										raw.audio_data.size(),
										nullptr, // auto-detect format
										&_decoder);
		if (result != MA_SUCCESS)
		{
			SFG_ERR("Failed to init decoder from memory for {0}", raw.name);
			return;
		}

		_flags.set(audio::flags::is_init);

		SFG_INFO("Created audio resource: {0} (stream={1})", raw.name, _flags.is_set(audio::flags::is_streaming));
	}

	void audio::destroy(chunk_allocator32& alloc)
	{
		SFG_ASSERT(_flags.is_set(audio::flags::is_init));

		ma_decoder_uninit(&_decoder);
		_flags.remove(audio::flags::is_init);
		if (_audio_data.size != 0)
			alloc.free(_audio_data);
		_audio_data = {};
	}

	// If streaming, we let ma_sound stream from decoder.
	// If not streaming, ma_sound will also work (just reads everything immediately).
	// ma_uint32 flags = _flags.is_set(audio::flags::is_streaming) ? MA_SOUND_FLAG_STREAM : 0;

	/*
	result = ma_sound_init_from_data_source(engine, &_decoder, flags, nullptr, &_sound);
	if (result != MA_SUCCESS)
	{
		SFG_ERR("Failed to init sound for {0}", raw.name);
		ma_decoder_uninit(&_decoder);
		return;
	}
	*/
	// ma_sound_uninit(&_sound);
}

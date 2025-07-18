// Copyright (c) 2025 Inan Evin

#include "compressor.hpp"
#include "io/log.hpp"
#include "data/istream.hpp"
#include "data/ostream.hpp"
#include <lz4/lz4.h>

namespace Game
{
	size_t estimate_decompressed_size(size_t compressedSize)
	{
		// return (compressedSize << 8) - compressedSize - 2526;
		return 255 * compressedSize + 24;
	}

	ostream compressor::compress(ostream& stream)
	{
		const uint32 streamSize		  = static_cast<uint32>(stream.get_size());
		const uint8	 shouldCompress	  = (streamSize < 150000000 && streamSize > 750000) ? 1 : 0;
		const uint32 uncompressedSize = streamSize + sizeof(uint8) + sizeof(uint32);

		stream << shouldCompress;
		stream << uncompressedSize;

		if (!shouldCompress)
		{
			ostream compressed;
			compressed.create(stream.get_size());
			compressed.write_raw(stream.get_raw(), stream.get_size());
			return compressed;
		}

		const int size			= static_cast<int>(stream.get_size());
		const int compressBound = LZ4_compressBound(size);

		// Create stream capable of holding max compressed bytes.
		ostream compressedStream = ostream();
		compressedStream.create(compressBound);
		char* dest		   = (char*)compressedStream.get_raw();
		char* data		   = (char*)stream.get_raw();
		int	  bytesWritten = LZ4_compress_default(data, dest, size, compressBound);

		if (bytesWritten == 0) { GAME_ERR("[compressor] -> LZ4 compression failed!"); }

		compressedStream.shrink(static_cast<size_t>(bytesWritten));
		return compressedStream;
	}

	istream compressor::decompress(istream& stream)
	{
		// Read uncompressed size of archive.
		uint8  shouldDecompress = 0;
		uint32 uncompressedSize = 0;
		stream.seek(stream.get_size() - sizeof(uint32) - sizeof(uint8));
		stream.read(shouldDecompress);
		stream.read(uncompressedSize);
		stream.seek(0);

		if (!shouldDecompress)
		{
			istream copy;
			copy.create(stream.get_raw(), stream.get_size() - sizeof(uint32) - sizeof(uint8));
			return copy;
		}

		const size_t size				= stream.get_size();
		istream		 decompressedStream = istream();
		decompressedStream.create(nullptr, uncompressedSize);
		void*	  src			   = stream.get_raw();
		void*	  ptr			   = decompressedStream.get_raw();
		const int decompressedSize = LZ4_decompress_safe((char*)src, (char*)ptr, static_cast<int>(size), static_cast<int>(uncompressedSize));
		decompressedStream.shrink(static_cast<size_t>(decompressedSize) - sizeof(uint32) - sizeof(uint8));
		return decompressedStream;
	}
}

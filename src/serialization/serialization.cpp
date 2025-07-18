// Copyright (c) 2025 Inan Evin

#include "serialization/serialization.hpp"
#include "io/log.hpp"
#include "io/file_system.hpp"
#include "data/ostream.hpp"
#include "data/istream.hpp"
#include "compressor.hpp"

#include <fstream>
#include <filesystem>

namespace Game
{
	bool Serialization::write_to_file(string_view fileInput, const char* targetFilePath)
	{
		std::ofstream outFile(targetFilePath);

		if (outFile.is_open())
		{
			outFile.write(fileInput.data(), fileInput.size());
			outFile.close();
		}
		else
		{
			GAME_ERR("Failed writing to file! {0}", targetFilePath);
			return false;
		}

		return true;
	}

	bool Serialization::save_to_file(const char* path, ostream& stream)
	{
		if (file_system::exists(path)) file_system::delete_file(path);

		std::ofstream wf(path, std::ios::out | std::ios::binary);

		if (!wf)
		{
			GAME_ERR("[Serialization] -> Could not open file for writing! {0}", path);
			return false;
		}

		ostream compressed = compressor::compress(stream);
		compressed.write_to_ofstream(wf);
		wf.close();
		compressed.destroy();

		if (!wf.good())
		{
			GAME_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
			return false;
		}

		return true;
	}

	istream Serialization::load_from_file(const char* path)
	{
		if (!file_system::exists(path))
		{
			GAME_ERR("[Serialization] -> File doesn't exists: {0}", path);
			return {};
		}

		std::ifstream rf(path, std::ios::out | std::ios::binary);

		if (!rf)
		{
			GAME_ERR("[Serialization] -> Could not open file for reading! {0}", path);
			return istream();
		}

		auto size = std::filesystem::file_size(path);

		// Create
		istream readStream;
		readStream.create(nullptr, size);
		readStream.read_from_ifstream(rf);
		rf.close();

		if (!rf.good())
		{
			GAME_ERR("[Serialization] -> Error occured while reading the file! {0}", path);
			readStream.destroy();
			return {};
		}

		istream decompressedStream = compressor::decompress(readStream);
		readStream.destroy();
		return decompressedStream;
	}

}

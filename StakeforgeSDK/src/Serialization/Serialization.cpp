/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Serialization/Serialization.hpp"
#include "SFG/IO/FileSystem.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Data/OStream.hpp"
#include "SFG/Data/IStream.hpp"
#include "SFG/Serialization/Compressor.hpp"
#include <fstream>
#include <filesystem>

namespace SFG
{
	bool Serialization::WriteToFile(const String& fileInput, const String& targetFilePath)
	{
		std::ofstream outFile(targetFilePath.c_str());

		if (outFile.is_open())
		{
			outFile << fileInput;
			outFile.close();
		}
		else
		{
			SFG_ERR("Failed writing to file! {0}", targetFilePath);
			return false;
		}

		return true;
	}

	bool Serialization::SaveToFile(const char* path, OStream& stream)
	{
		if (FS::Exists(path))
			FS::DeleteFileInPath(path);

		std::ofstream wf(path, std::ios::out | std::ios::binary);

		if (!wf)
		{
			SFG_ERR("[Serialization] -> Could not open file for writing! {0}", path);
			return false;
		}

		OStream compressed = Compressor::Compress(stream);
		compressed.WriteToOFStream(wf);
		wf.close();
		compressed.Destroy();

		if (!wf.good())
		{
			SFG_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
			return false;
		}

		return true;
	}

	IStream Serialization::LoadFromFile(const char* path)
	{
		if (!FS::Exists(path))
		{
			SFG_ERR("[Serialization] -> File doesn't exists: {0}", path);
			return {};
		}

		std::ifstream rf(path, std::ios::out | std::ios::binary);

		if (!rf)
		{
			SFG_ERR("[Serialization] -> Could not open file for reading! {0}", path);
			return IStream();
		}

		auto size = std::filesystem::file_size(path);

		// Create
		IStream readStream;
		readStream.Create(nullptr, size);
		readStream.ReadFromIFStream(rf);
		rf.close();

		if (!rf.good())
		{
			SFG_ERR("[Serialization] -> Error occured while reading the file! {0}", path);
			readStream.Destroy();
			return {};
		}

		IStream decompressedStream = Compressor::Decompress(readStream);
		readStream.Destroy();
		return decompressedStream;
	}

} // namespace SFG

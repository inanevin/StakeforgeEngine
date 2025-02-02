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

#include "SFG/IO/FileSystem.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Data/StringUtil.hpp"

#include <filesystem>
#include <fstream>

#ifdef SFG_PLATFORM_OSX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#ifdef SFG_PLATFORM_WINDOWS
#include <shlobj.h>
#endif

namespace SFG
{
	bool FS::DeleteFileInPath(const char* path)
	{
		return remove(path);
	}

	bool FS::CreateFolderInPath(const char* path)
	{
		Vector<String> directories = {};
		StringUtil::SplitBy(directories, path, "/");

		String currentPath = "";
		for (const auto& dir : directories)
		{
			currentPath += dir + "/";

			if (!Exists(currentPath.c_str()))
			{
				bool success = std::filesystem::create_directory(currentPath.c_str());
				if (!success)
				{
					SFG_ERR("Could not create directory: {0}", currentPath);
					return false;
				}
			}
		}
		return true;
	}

	bool FS::DeleteDirectory(const char* path)
	{
		try
		{
			bool success = std::filesystem::remove_all(path);
		}
		catch (const std::exception& err)
		{
			SFG_ERR("Could not delete directory: {0}, {1}", path, err.what());
			return false;
		}

		return true;
	}

	void FS::GetFilesInDirectory(const char* path, Vector<String>& outData, String extensionFilter)
	{
		outData.clear();
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (!entry.is_directory())
			{
				if (extensionFilter.empty())
				{
					String path = entry.path().string().c_str();
					outData.push_back(FixPath(path));
				}
				else
				{
					String fullpath = entry.path().string().c_str();
					if (GetFileExtension(fullpath).compare(extensionFilter))
					{
						outData.push_back(FixPath(fullpath));
					}
				}
			}
		}
	}

	void FS::GetFilesAndFoldersInDirectory(const char* path, Vector<String>& outData)
	{
		outData.clear();
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			outData.push_back(FixPath(entry.path().string().c_str()));
		}
	}

	bool FS::IsDirectory(const char* path)
	{
		return std::filesystem::is_directory(path);
	}

	bool FS::ChangeDirectoryName(const char* oldPath, const char* newPath)
	{
		if (std::rename(oldPath, newPath) != 0)
		{
			SFG_ERR("Failed to rename directory! Old Name: {0}, New Name: {1}", oldPath, newPath);
			return false;
		}

		return true;
	}

	bool FS::Exists(const char* path)
	{
		return std::filesystem::exists(path);
	}

	String FS::GetLastModifiedDate(const char* path)
	{
		std::filesystem::file_time_type ftime  = std::filesystem::last_write_time(path);
		auto							sctp   = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
		std::time_t						cftime = std::chrono::system_clock::to_time_t(sctp);
		return std::asctime(std::localtime(&cftime));
	}

	String FS::GetFileDirectory(const char* path)
	{
		String		 str	   = path;
		const char*	 cstr	   = path;
		unsigned int strLength = (unsigned int)str.length();
		unsigned int end	   = strLength - 1;

		while (end != 0)
		{
			if (cstr[end] == '/')
				break;

			end--;
		}

		if (end == 0)
			return str;

		else
		{
			unsigned int start = 0;
			end				   = end + 1;
			return str.substr(start, end - start).data();
		}
	}

	String FS::RemoveExtensionFromPath(const String& fileName)
	{
		const size_t lastindex = fileName.find_last_of(".");
		return FixPath(fileName.substr(0, lastindex));
	}

	String FS::GetFilenameAndExtensionFromPath(const String& fileName)
	{
		return FixPath(fileName.substr(fileName.find_last_of("/\\") + 1));
	}

	String FS::GetFileExtension(const String& file)
	{
		return FixPath(file.substr(file.find_last_of(".") + 1));
	}

	String FS::GetFilenameOnlyFromPath(const String& file)
	{
		return RemoveExtensionFromPath(GetFilenameAndExtensionFromPath(file));
	}

	String FS::GetLastFolderFromPath(const String& path)
	{
		String		 fixedPath = FixPath(path);
		const size_t lastSlash = fixedPath.find_last_of("/\\");

		if (lastSlash == fixedPath.size() || lastSlash == fixedPath.size() - 1)
			fixedPath = fixedPath.substr(0, lastSlash);

		const size_t actualLast = fixedPath.find_last_of("/\\");
		if (actualLast != String::npos)
			fixedPath = fixedPath.substr(actualLast + 1, fixedPath.size());
		return fixedPath;
	}

	String FS::ReadFileContentsAsString(const String& filePath)
	{
		std::ifstream ifs(filePath.c_str());
		auto		  a = std::istreambuf_iterator<char>(ifs);
		auto		  b = (std::istreambuf_iterator<char>());
		return std::string(a, b);
	}

	void FS::ReadFileContentsToVector(const char* filePath, Vector<char>& vec)
	{
		std::ifstream file(filePath, std::ios::binary);
		if (!file)
		{
			SFG_ERR("[Font] -> Could not open file! {0}", filePath);
			return;
		}

		// Size
		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);

		// Into vec
		vec = Vector<char>(length);
		file.read(&vec[0], length);
	}

	String FS::GetRunningDirectory()
	{
		try
		{
			auto path = std::filesystem::current_path();
			return path.string();
		}
		catch (std::exception e)
		{
			int a = 5;
			return "";
		}
	}

	String FS::GetUserDataFolder()
	{
#ifdef SFG_PLATFORM_WINDOWS
		char path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
		{
			String pp = String(path);
			return pp;
		}
		return std::string();
#elif defined SFG_PLATFORM_OSX
		struct passwd* pw	= getpwuid(getuid());
		String		   path = String(pw->pw_dir);
		return path + "/Library/Application Support/";
#endif
		return "";
	}

	String FS::FixPath(const String& str)
	{
		String result = StringUtil::ReplaceAll(str, "\\", "/");
		result		  = StringUtil::ReplaceAll(result, "\\\\", "/");
		// result = UtilStr::ReplaceAll(result, "\\", "/");
		return result;
	}

	namespace
	{
		void SFGCopyFile(const std::filesystem::path& source, const std::filesystem::path& destination)
		{
			try
			{
				std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				SFG_ERR("Error duplicating file! {0}", e.what());
			}
		}

		void SFGCopyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination)
		{
			try
			{
				std::filesystem::create_directory(destination);
				for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
				{
					const auto& path			= entry.path();
					auto		relativePathStr = path.lexically_relative(source).string();
					std::filesystem::copy(path, destination / relativePathStr, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
			}
			catch (std::filesystem::filesystem_error& e)
			{
				SFG_ERR("Error duplicating directory! {0}", e.what());
			}
		}
	} // namespace

	String FS::Duplicate(const char* path)
	{
		try
		{
			if (std::filesystem::exists(path))
			{
				const bool	 isDirectory   = FS::IsDirectory(path);
				const String correctedPath = isDirectory ? path : FS::RemoveExtensionFromPath(path);
				String		 finalPath	   = correctedPath + " (Copy)";

				size_t insertBeforeExtension = finalPath.length();
				if (!isDirectory)
				{
					finalPath += "." + FS::GetFileExtension(path);
				}

				while (FS::Exists(finalPath.c_str()))
				{
					finalPath.insert(insertBeforeExtension, " (Copy)");
					insertBeforeExtension += 7;
				}

				std::filesystem::path destination = finalPath;

				if (std::filesystem::is_regular_file(path))
				{
					SFGCopyFile(path, destination);
					return finalPath;
				}
				else if (std::filesystem::is_directory(path))
				{
					SFGCopyDirectory(path, destination);
					return finalPath;
				}
				else
				{
					SFG_ERR("Unsupported file type! {0}", path);
				}
			}
			else
			{
				SFG_ERR("Path doesn't exist! {0}", path);
			}
		}
		catch (std::filesystem::filesystem_error&)
		{
			SFG_ERR("Exception processing path! {0}", path);
		}

		return "";
	}

	String FS::GetRelative(const char* src, const char* target)
	{
		std::filesystem::path sourcePath(src);
		std::filesystem::path targetPath(target);
		std::filesystem::path relativePath = std::filesystem::relative(targetPath, sourcePath);
		return relativePath.string();
	}

	void FS::PerformMove(const char* targetFile, const char* targetDirectory)
	{
		try
		{
			// Create the directory if it doesn't exist
			if (!std::filesystem::exists(targetDirectory))
			{
				SFG_ERR("Target directory does not exist! {0}", targetDirectory);
				return;
			}

			// Construct the new path for the file
			std::filesystem::path file		  = std::filesystem::path(targetFile);
			std::filesystem::path destination = std::filesystem::path(targetDirectory) / file.filename();

			// Move (or rename) the file to the new directory
			std::filesystem::rename(targetFile, destination);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			SFG_ERR("::PerformMove error {0}", e.what());
		}
	}

	String FS::GetSystemTimeStr()
	{
		std::time_t		   now		 = std::time(nullptr);
		std::tm*		   localTime = std::localtime(&now);
		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":" << std::setw(2) << std::setfill('0') << localTime->tm_min << ":" << std::setw(2) << std::setfill('0') << localTime->tm_sec;
		return oss.str();
	}

	void FS::GetSystemTimeInts(int32& hours, int32& minutes, int32& seconds)
	{
		std::time_t now		  = std::time(nullptr);
		std::tm*	localTime = std::localtime(&now);
		hours				  = localTime->tm_hour;
		minutes				  = localTime->tm_min;
		seconds				  = localTime->tm_sec;
	}

	String FS::GetTimeStrFromMicroseconds(int64 microseconds)
	{
		int64 totalSeconds = microseconds / 1000000;

		int hours	= (totalSeconds / 3600) % 24; // Wrap-around using modulo 24
		int minutes = (totalSeconds / 60) % 60;
		int seconds = totalSeconds % 60;

		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

		return oss.str();
	}

	void FS::CopyDirectory(const char* directory, const char* targetParentFolder)
	{
		namespace fs = std::filesystem;

		try
		{
			// Ensure the source directory exists
			fs::path source(directory);
			if (!fs::exists(source))
			{
				throw std::runtime_error("Source directory does not exist or is not a directory.");
			}

			// Create the destination path
			fs::path destination = fs::path(targetParentFolder) / source.filename();

			// Check if the destination already exists
			if (fs::exists(destination))
				fs::remove_all(destination);

			// Recursively copy the directory and its contents
			fs::create_directories(destination); // Create the target folder
			for (const auto& entry : fs::recursive_directory_iterator(source))
			{
				const auto& sourcePath		= entry.path();
				auto		relativePath	= fs::relative(sourcePath, source);
				auto		destinationPath = destination / relativePath;

				if (fs::is_directory(sourcePath))
					fs::create_directory(destinationPath);
				else if (fs::is_regular_file(sourcePath))
					fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
			}
		}
		catch (const std::exception& ex)
		{
			SFG_ERR("Error while copying directory {0}", ex.what());
		}
	}

	void FS::CopyFileToDirectory(const char* file, const char* targetParentFolder)
	{
		try
		{
			std::filesystem::path sourceFile		= file;
			std::filesystem::path destinationFolder = targetParentFolder;

			// Ensure source file exists
			if (!std::filesystem::exists(sourceFile) || !std::filesystem::is_regular_file(sourceFile))
			{
				SFG_ERR("Error: Source file does not exist or is not a valid file. {0} {1}", file, targetParentFolder);
				return;
			}

			// Ensure target directory exists
			if (!std::filesystem::exists(destinationFolder))
			{
				SFG_ERR("Error: Target directory does not exist. {0} {1}", file, targetParentFolder);
				return;
			}

			if (!std::filesystem::is_directory(destinationFolder))
			{
				SFG_ERR("Error: Target path is not a directory. {0} {1}", file, targetParentFolder);
				return;
			}

			// Construct destination path
			std::filesystem::path destinationFile = destinationFolder / sourceFile.filename();

			// Copy file
			std::filesystem::copy_file(sourceFile, destinationFile, std::filesystem::copy_options::overwrite_existing);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			SFG_ERR("Filesystem error : {0} { 1 }", file, targetParentFolder);
		}
	}

} // namespace SFG

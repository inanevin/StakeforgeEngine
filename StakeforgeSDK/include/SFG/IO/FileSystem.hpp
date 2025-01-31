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

#pragma once

#include "SFG/Data/Vector.hpp"
#include "SFG/Data/String.hpp"

namespace SFG
{
	class FS
	{
	public:
		/// <summary>
		/// Use only for files.
		/// </summary>
		/// <returns></returns>
		static bool DeleteFileInPath(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static bool CreateFolderInPath(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static bool DeleteDirectory(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <param name="outData"></param>
		/// <param name="extensionFilter"></param>
		static void GetFilesInDirectory(const char* path, Vector<String>& outData, String extensionFilter = "");

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <param name="outData"></param>
		static void GetFilesAndFoldersInDirectory(const char* path, Vector<String>& outData);

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static bool IsDirectory(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="oldPath"></param>
		/// <param name="newPath"></param>
		/// <returns></returns>
		static bool ChangeDirectoryName(const char* oldPath, const char* newPath);

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static bool Exists(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static String GetLastModifiedDate(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="fileName"></param>
		/// <returns></returns>
		static String GetFileDirectory(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		static String RemoveExtensionFromPath(const String& filename);

		/// <summary>
		///
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		static String GetFilenameAndExtensionFromPath(const String& filename);

		/// <summary>
		///
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		static String GetFileExtension(const String& file);

		/// <summary>
		///
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		static String GetFilenameOnlyFromPath(const String& file);

		/// <summary>
		///
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static String GetLastFolderFromPath(const String& path);

		/// <summary>
		///
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		static String ReadFileContentsAsString(const String& filePath);

		/// <summary>
		/// Of the current executable running Stakeforge library.
		/// </summary>
		/// <returns></returns>
		static String GetRunningDirectory();

		/// <summary>
		/// %AppData% in Windows, Application Support on OSX.
		/// </summary>
		/// <returns></returns>
		static String GetUserDataFolder();

		/// <summary>
		/// Replaces all backslashes with forward slashes
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String FixPath(const String& str);

		/// <summary>
		/// Duplicate a folder or a directory.
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static String Duplicate(const char* path);

		/// <summary>
		///
		/// </summary>
		/// <param name="src"></param>
		/// <param name="target"></param>
		/// <returns></returns>
		static String GetRelative(const char* src, const char* target);

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static String GetSystemTimeStr();

		/// <summary>
		///
		/// </summary>
		/// <param name="microseconds"></param>
		/// <returns></returns>
		static String GetTimeStrFromMicroseconds(int64 microseconds);

		/// <summary>
		///
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="vec"></param>
		static void ReadFileContentsToVector(const char* filePath, Vector<char>& vec);

		/// <summary>
		///
		/// </summary>
		/// <param name="targetFile"></param>
		/// <param name="targetDirectory"></param>
		static void PerformMove(const char* targetFile, const char* targetDirectory);

		/// <summary>
		///
		/// </summary>
		/// <param name="hours"></param>
		/// <param name="minutes"></param>
		/// <param name="seconds"></param>
		static void GetSystemTimeInts(int32& hours, int32& minutes, int32& seconds);

		/// <summary>
		///
		/// </summary>
		/// <param name="directory"></param>
		/// <param name="targetParentFolder"></param>
		static void CopyDirectory(const char* copyDir, const char* targetParentFolder);
	};

} // namespace SFG

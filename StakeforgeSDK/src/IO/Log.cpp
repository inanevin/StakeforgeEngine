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

#include "SFG/IO/Log.hpp"
#include "SFG/Data/String.hpp"

#ifdef SFG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace SFG
{
	void Log::LogImpl(Log::Level level, const char* msg)
	{
		LOCK_GUARD(m_logMtx);

		String msgStr = "[" + String(GetLevel(level)) + "] " + msg + "\n";

#ifdef SFG_PLATFORM_WINDOWS
		HANDLE hConsole;
		int	   color = 15;

		if ((level & Level::LOG_LEVEL_TRACE))
			color = 3;
		else if ((level & Level::LOG_LEVEL_INFO))
			color = 15;
		else if ((level & Level::LOG_LEVEL_WARNING))
			color = 6;
		else if ((level & Level::LOG_LEVEL_ERROR))
			color = 4;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msgStr.c_str(), static_cast<DWORD>(strlen(msgStr.c_str())), NULL, NULL);
#else
		std::cout << msgStr.c_str();
#endif
	}

	const char* Log::GetLevel(Level level)
	{
		switch (level)
		{
		case Level::LOG_LEVEL_ERROR:
			return "Error";
		case Level::LOG_LEVEL_INFO:
			return "Info";
		case Level::LOG_LEVEL_TRACE:
			return "Trace";
		case Level::LOG_LEVEL_WARNING:
			return "Warn";
		default:
			return "";
		}
	}

} // namespace SFG

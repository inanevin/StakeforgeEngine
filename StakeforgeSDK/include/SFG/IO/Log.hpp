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

#ifdef SFG_DEBUG

#define SFG_ERR(...)   SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_ERROR, __VA_ARGS__)
#define SFG_WARN(...)  SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_WARNING, __VA_ARGS__)
#define SFG_INFO(...)  SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_INFO, __VA_ARGS__)
#define SFG_TRACE(...) SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_TRACE, __VA_ARGS__)

#else

#define SFG_ERR(...)   SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_ERROR, __VA_ARGS__)
#define SFG_WARN(...)  SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_WARNING, __VA_ARGS__)
#define SFG_INFO(...)  SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_INFO, __VA_ARGS__)
#define SFG_TRACE(...) SFG::Log::Instance().LogMessage(SFG::Log::Level::LOG_LEVEL_TRACE, __VA_ARGS__)

#endif

#ifdef SFG_PLATFORM_WINDOWS
#define DBG_BRK __debugbreak();
#else
#define DBG_BRK __builtin_trap();
#endif

#ifdef SFG_DEBUG
#define SFG_ASSERT(x, ...)                                                                                                                                                                                                                                         \
	if (!(x))                                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                              \
		SFG_ERR(__VA_ARGS__);                                                                                                                                                                                                                                      \
		DBG_BRK                                                                                                                                                                                                                                                    \
	}

#define SFG_ASSERT_F(x)                                                                                                                                                                                                                                            \
	if (!(x))                                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                              \
		DBG_BRK                                                                                                                                                                                                                                                    \
	}

#else
#define SFG_ASSERT(x, ...)
#endif

#define SFG_NOTIMPLEMENTED SFG_ASSERT(false, "Implementation missing!")

#include "SFG/Data/Mutex.hpp"
#include "SFG/StakeforgeAPI.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace SFG
{
	class SFG_API Log
	{
	public:
		enum Level
		{
			LOG_LEVEL_INFO	  = 1 << 0,
			LOG_LEVEL_ERROR	  = 1 << 1,
			LOG_LEVEL_TRACE	  = 1 << 2,
			LOG_LEVEL_WARNING = 1 << 3,
		};

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static Log& Instance()
		{
			static Log log;
			return log;
		}

		// Helper to convert various types to string
		template <typename T> std::string ToString(const T& value)
		{
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		template <typename... Args> std::string FormatString(const std::string& format, const Args&... args)
		{
			std::vector<std::string> argList{ToString(args)...}; // Convert args to strings
			std::ostringstream		 result;
			size_t					 i = 0;

			while (i < format.size())
			{
				if (format[i] == '{')
				{
					size_t end = format.find('}', i);
					if (end != std::string::npos)
					{
						std::string indexStr = format.substr(i + 1, end - i - 1);
						try
						{
							size_t index = std::stoul(indexStr);
							if (index < argList.size())
							{
								result << argList[index]; // Replace with corresponding argument
							}
							else
							{
								result << "{" << indexStr << "}"; // Keep original if out of bounds
							}
						}
						catch (...)
						{
							result << "{" << indexStr << "}"; // Handle invalid indices
						}
						i = end + 1;
						continue;
					}
				}
				result << format[i++];
			}

			return result.str();
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="level"></param>
		/// <param name="...args"></param>
		template <typename... Args> void LogMessage(Level level, const Args&... args)
		{
			LogImpl(level, FormatString(args...).c_str());
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline Mutex& GetLogMutex()
		{
			return m_logMtx;
		}

	private:
		void		LogImpl(Level level, const char* msg);
		const char* GetLevel(Level level);

	private:
		Mutex m_logMtx;
	};
} // namespace SFG

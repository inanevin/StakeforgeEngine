// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef GAME_DEBUG

#define GAME_ERR(...)	Game::log::instance().log_msg(Game::log::level::error, __VA_ARGS__)
#define GAME_WARN(...)	Game::log::instance().log_msg(Game::log::level::warning, __VA_ARGS__)
#define GAME_INFO(...)	Game::log::instance().log_msg(Game::log::level::info, __VA_ARGS__)
#define GAME_TRACE(...) Game::log::instance().log_msg(Game::log::level::trace, __VA_ARGS__)

#else

#define GAME_ERR(...)	Game::log::instance().log_msg(Game::log::level::error, __VA_ARGS__)
#define GAME_WARN(...)	Game::log::instance().log_msg(Game::log::level::warning, __VA_ARGS__)
#define GAME_INFO(...)	Game::log::instance().log_msg(Game::log::level::info, __VA_ARGS__)
#define GAME_TRACE(...) Game::log::instance().log_msg(Game::log::level::trace, __VA_ARGS__)

#endif

#include "data/mutex.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Game
{
	class log
	{
	public:
		enum class level
		{
			info,
			error,
			trace,
			warning,
		};

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static log& instance()
		{
			static log log;
			return log;
		}

		// Helper to convert various types to string
		template <typename T> std::string to_str(const T& value)
		{
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		template <typename... Args> std::string format_str(const std::string& format, const Args&... args)
		{
			std::vector<std::string> argList{to_str(args)...}; // Convert args to strings
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
		template <typename... Args> void log_msg(level level, const Args&... args)
		{
			log_impl(level, format_str(args...).c_str());
		}

	private:
		const char* get_level(level lvl);
		void		log_impl(level level, const char* msg);

	private:
		mutex _mtx;
	};
}

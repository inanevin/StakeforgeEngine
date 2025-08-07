// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef GAME_DEBUG

#define GAME_ERR(...)	Game::log::instance().log_msg(Game::log_level::error, __VA_ARGS__)
#define GAME_WARN(...)	Game::log::instance().log_msg(Game::log_level::warning, __VA_ARGS__)
#define GAME_INFO(...)	Game::log::instance().log_msg(Game::log_level::info, __VA_ARGS__)
#define GAME_TRACE(...) Game::log::instance().log_msg(Game::log_level::trace, __VA_ARGS__)
#define GAME_FATAL(...) Game::log::instance().log_msg(Game::log_level::error, __VA_ARGS__)
#define GAME_PROG(...)	Game::log::instance().log_msg(Game::log_level::progress, __VA_ARGS__)

#else

#define GAME_ERR(...)	Game::log::instance().log_msg(Game::log_level::error, __VA_ARGS__)
#define GAME_WARN(...)	Game::log::instance().log_msg(Game::log_level::warning, __VA_ARGS__)
#define GAME_INFO(...)	Game::log::instance().log_msg(Game::log_level::info, __VA_ARGS__)
#define GAME_TRACE(...) Game::log::instance().log_msg(Game::log_level::trace, __VA_ARGS__)
#define GAME_FATAL(...) Game::log::instance().log_msg(Game::log_level::error, __VA_ARGS__)
#define GAME_PROG(...)	Game::log::instance().log_msg(Game::log_level::progress, __VA_ARGS__)

#endif

#include "data/mutex.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "data/vector.hpp"
#include "memory/malloc_allocator_stl.hpp"
#include <functional>

namespace Game
{
	enum class log_level
	{
		info,
		error,
		trace,
		warning,
		progress,
	};

	class log
	{
	public:
		typedef std::function<void(log_level lvl, const char* msg)> callback_function;

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
		template <typename... Args> void log_msg(log_level level, const Args&... args)
		{
			log_impl(level, format_str(args...).c_str());
		}

		void add_listener(unsigned int id, callback_function f);
		void remove_listener(unsigned int id);

	private:
		struct listener
		{
			unsigned int	  id = 0;
			callback_function f	 = nullptr;
		};

	private:
		const char* get_level(log_level lvl);
		void		log_impl(log_level level, const char* msg);

	private:
		template <typename T> using vector_malloc = std::vector<T, malloc_allocator_stl<T>>;

		mutex					_mtx;
		vector_malloc<listener> _listeners;
	};
}

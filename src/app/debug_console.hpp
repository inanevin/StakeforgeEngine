// Copyright (c) 2025 Inan Evin
#pragma once
#include "common/size_definitions.hpp"
#include "common/system_info.hpp"
#include "data/string_id.hpp"
#include "data/hash_map.hpp"
#include "data/string.hpp"
#include "data/string_util.hpp"
#include "io/log.hpp"
#include "data/vector.hpp"
#include <functional>

namespace SFG
{
	namespace detail
	{
		template <typename T> static inline bool cvar_convert(const string& str, T& out)
		{
			static_assert(sizeof(T) == 0, "cVarConvert not implemented for this type");
			return false;
		}

		template <> bool cvar_convert<int>(const string& str, int& out)
		{
			try
			{
				out = std::stoi(str);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
		template <> bool cvar_convert<unsigned int>(const string& str, unsigned int& out)
		{
			try
			{
				out = std::stoul(str);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
		template <> bool cvar_convert<float>(const string& str, float& out)
		{
			try
			{
				out = std::stof(str);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
		template <> bool cvar_convert<double>(const string& str, double& out)
		{
			try
			{
				out = std::stod(str);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		template <> bool cvar_convert<string>(const string& str, string& out)
		{
			out = str;
			return true;
		}

		template <> bool cvar_convert<const char*>(const string& str, const char*& out)
		{
			out = str.c_str();
			return true;
		}

		template <typename... TArgs, size_t... Is> bool conver_args_impl(const vector<string>& parts, std::tuple<TArgs...>& outArgs, std::index_sequence<Is...>)
		{
			if constexpr (sizeof...(TArgs) > 0)
			{
				bool success[] = {detail::cvar_convert<TArgs>(parts[Is], std::get<Is>(outArgs))...};
				for (bool b : success)
					if (!b)
						return false;
			}
			return true;
		}
		template <typename... TArgs> bool parse_args(const vector<string>& parts, std::tuple<TArgs...>& outArgs)
		{
			if (parts.size() != sizeof...(TArgs))
				return false;
			return conver_args_impl<TArgs...>(parts, outArgs, std::index_sequence_for<TArgs...>{});
		}
	}

	class console_entry_base
	{
	public:
		virtual void execute(const string& str) = 0;
	};

	template <typename T> class console_variable : public console_entry_base
	{
	public:
		typedef std::function<void(T)> callback_function;
		console_variable(T data, callback_function cb) : _data(data), _callback(cb){};
		virtual ~console_variable() = default;

		T get_value() const
		{
			return _data;
		}

		const T& get_value_ref() const
		{
			return _data;
		}

		void execute(const string& args) override
		{
			if (args.empty())
			{
				SFG_ERR("console_variable::execute() -> no argument provided for the console variable");
				return;
			}

			T value;

			if (!detail::cvar_convert<T>(args, value))
			{
				SFG_ERR("console_variable::execute() -> failed to convert input to target type!");
				return;
			}

			_data = value;
			if (_callback)
				_callback(_data);
		}

	private:
		T				  _data = T();
		callback_function _callback;
	};

	template <typename... TArgs> class console_function : public console_entry_base
	{
	public:
		typedef std::function<void(TArgs... args)> function_type;

		console_function(function_type func) : _func(func){};
		virtual ~console_function() = default;

		template <typename func, typename tuple, size_t... Is> void call_func_from_tuple(func&& f, tuple& tup, std::index_sequence<Is...>)
		{
			f(std::get<Is>(tup)...);
		}

		virtual void execute(const string& args) override
		{
			if (args.empty())
			{
				std::tuple<TArgs...> parsed_args;
				call_func_from_tuple(_func, parsed_args, std::index_sequence_for<TArgs...>{});
			}

			vector<string> tokens;
			string_util::split(tokens, args, ",");

			std::tuple<TArgs...> parsed_args;
			if (!detail::parse_args(tokens, parsed_args))
			{
				SFG_ERR("console_funcion::execute() -> argument count mismatch or conversion failed!");
				return;
			}

			call_func_from_tuple(_func, parsed_args, std::index_sequence_for<TArgs...>{});
		}

	private:
		function_type _func;
	};

	class debug_console
	{
	public:
		static debug_console* get()
		{
			return s_instance;
		}

		template <typename... TArgs> void register_console_function(const char* name, typename console_function<TArgs...>::function_type cb)
		{
			VERIFY_INIT();

			const string_id sid = TO_SIDC(name);

			if (_console_entries.contains(sid))
			{
				SFG_ERR("debug_console::register_console_function() -> Console function {0} already exists.", name);
				return;
			}

			_console_entries[sid] = new console_function(cb);
		}

		template <typename T> void register_console_variable(const char* name, T initial_value, typename console_variable<T>::callback_function cb = nullptr)
		{
			VERIFY_INIT();

			const uint32 sid = TO_SIDC(name);
			if (_console_entries.find(sid) != _console_entries.end())
			{
				SFG_ERR("debug_console::register_console_variable() -> Console variable {0} already exists.", name);
				return;
			}

			_console_entries[sid] = new console_variable(initial_value, cb);
		}

		template <typename T> T get_console_variable(const char* name)
		{

			const uint32 sid = TO_SIDC(name);
			if (_console_entries.find(sid) == _console_entries.end())
			{
				SFG_ERR("debug_console::get_console_variable() -> Console variable {0} doesn't exist.", name);
				return;
			}

			return static_cast<console_variable<T>*>(_console_entries[sid])->GetValue();
		}

		void parse_console_command(const char* cmd);

	private:
		friend class game_app;
		static void init();
		static void uninit();

	private:
		static debug_console* s_instance;

		hash_map<string_id, console_entry_base*> _console_entries;
	};
}
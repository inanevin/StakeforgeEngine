// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/string.hpp"

namespace SFG
{
	class engine_data
	{
	public:
		static engine_data& get()
		{
			static engine_data inst;
			return inst;
		}

		void init();
		void uninit();
		void load();
		void save();

		inline const string& get_working_dir() const
		{
			return _working_dir;
		}

		inline const string& get_last_world() const
		{
			return _last_world;
		}

		inline void set_last_world(const string& p)
		{
			_last_world = p;
		}

	private:
		void report();

	private:
		string _working_dir = {};
		string _last_world	= {};
	};
}

// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/string.hpp"

namespace Game
{
	class string_util
	{
	public:
		static string		  remove_all_except_first(const string& str, const string& delimiter);
		static void			  append_float(float value, char* target_bufffer, uint32 significant_digits, uint32 decimals, bool null_term);
		static void			  replace_all(string& str, const string& to_replace, const string& replacement);
		static void			  to_upper(string& str);
		static void			  to_lower(string& str);
		static void			  remove_whitespace(string& str);
		static wstring		  to_wstr(const string& string);
		static void			  split(vector<string>& out, const string& str, const string& split);
		static char*		  wchar_to_char(const wchar_t* wch);
		static const wchar_t* char_to_wchar(const char* ch);
		static float		  to_float(const string& str, uint32& out_decimals, char seperator = '.');
		static int			  to_int(const string& str);
		static uint64		  to_big_int(const string& str);
	};

}

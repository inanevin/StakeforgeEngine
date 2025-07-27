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
		static string		  wstr_to_str(const wstring& wstring);
		static string		  wchar_to_str(wchar_t wch);
		static string		  encode_utf8(wchar_t wch);
		static string		  remove_all_except_first(const string& str, const string& delimiter);
		static string		  fix_number(const string& str);
		static string		  from_float(float val, uint32 decimals);
		static string		  replace_all(const string& str, const string& to_replace, const string& replacement);
		static string		  get_until_first_of(const string& str, const string& find);
		static string		  to_upper(const string& str);
		static string		  to_lower(const string& str);
		static string		  remove_whitespace(const string& str);
		static wstring		  to_wstr(const string& string);
		static void			  split(vector<string>& out, const string& str, const string& split);
		static char*		  wchar_to_char(const wchar_t* wch);
		static const wchar_t* char_to_wchar(const char* ch);
		static float		  to_float(const string& str, uint32& out_decimals);
		static int			  to_int(const string& str);
		static uint64		  to_big_int(const string& str);
		static string		  from_uint(uint32 val, uint8 max_digits);
	};

}

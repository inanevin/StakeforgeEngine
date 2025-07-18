// Copyright (c) 2025 Inan Evin

#include "string_util.hpp"

#include <charconv>
#include <codecvt>
#include <locale>
#include <iostream>
#include <cwchar>
#include <cstring>
#include <algorithm>

#ifdef GAME_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4333)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace Game
{

	string string_util::wstr_to_str(const wstring& wstring)
	{
		std::wstring											  wstr = wstring.c_str();
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(wstr);
	}

	string string_util::wchar_to_str(wchar_t wch)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.to_bytes(wch);
	}

	wstring string_util::to_wstr(const string& string)
	{
		std::string												  str = string.c_str();
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.from_bytes(str);
	}

	char* string_util::wchar_to_char(const wchar_t* wch)
	{
		// Count required buffer size (plus one for null-terminator).
		size_t size	  = (wcslen(wch) + 1) * sizeof(wchar_t);
		char*  buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
		// wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
		size_t convertedSize;
		std::wcstombs_s(&convertedSize, buffer, size, input, size);
#else
#pragma warning(disable : 4996)
		std::wcstombs(buffer, wch, size);
#endif
		return buffer;
	}

	const wchar_t* string_util::char_to_wchar(const char* ch)
	{
#ifdef GAME_PLATFORM_WINDOWS
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring									 wide_str = converter.from_bytes(ch);

		wchar_t* wide_copy = new wchar_t[wide_str.size() + 1];
		wcscpy_s(wide_copy, wide_str.size() + 1, wide_str.c_str());

		return wide_copy;
#endif

#ifdef GAME_PLATFORM_OSX
		// Convert the input char string to a wchar_t string on Apple platform
		size_t	 length	   = strlen(ch);
		wchar_t* wide_copy = new wchar_t[length + 1];

		mbstowcs(wide_copy, ch, length);
		wide_copy[length] = L'\0'; // Null-terminate the wide string

		return wide_copy;
#endif
	}

	string string_util::encode_utf8(wchar_t ch)
	{
		string utf8;
		if (ch < 0x80) { utf8.push_back(static_cast<char>(ch)); }
		else if (ch < 0x800)
		{
			utf8.push_back(static_cast<char>(0xC0 | (ch >> 6)));
			utf8.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else if (ch < 0x10000)
		{
			utf8.push_back(static_cast<char>(0xE0 | (ch >> 12)));
			utf8.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			utf8.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else
		{
			utf8.push_back(static_cast<char>(0xF0 | (ch >> 18)));
			utf8.push_back(static_cast<char>(0x80 | ((ch >> 12) & 0x3F)));
			utf8.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			utf8.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		return utf8;
	}

	string string_util::replace_all(const string& str, const string& to_replace, const string& replacement)
	{
		if (to_replace.empty()) return str;

		string fin		 = str;
		size_t start_pos = 0;
		while ((start_pos = fin.find(to_replace, start_pos)) != std::string::npos)
		{
			fin.replace(start_pos, to_replace.length(), replacement);
			start_pos += replacement.length();
		}
		return fin;
	}
	float string_util::to_float(const string& str, uint32& outDecimals)
	{
		try
		{
			const string fin = string_util::replace_all(str, ",", ".");
			std::size_t	 pos = fin.find('.');
			if (pos != std::string::npos) outDecimals = static_cast<uint32>(fin.length() - pos - 1);

			return std::stof(fin);
		}
		catch (const std::exception& e)
		{
			// GAME_ERR("Exception: to_float() string: {0} - decimals: {1} - {2}", str, outDecimals, e.what());
			return 0.0f;
		}
	}

	int string_util::to_int(const string& str)
	{
		try
		{
			return std::stoi(str);
		}
		catch (const std::exception& e)
		{
			// GAME_ERR("Exception: to_int() string: {0} - {1}", str, e.what());
			return 0;
		}
	}

	uint64 string_util::to_big_int(const string& str)
	{
		try
		{
			return static_cast<uint64>(std::stoull(str));
		}
		catch (const std::exception& e)
		{
			// GAME_ERR("Exception: to_int() string: {0} - {1}", str, e.what());
			return 0;
		}
	}

	string string_util::remove_all_except_first(const string& str, const string& delimiter)
	{
		string		result = str;
		std::size_t pos	   = result.find(delimiter); // find the first dot

		// if there is a dot in the string
		if (pos != std::string::npos)
		{
			// remove all subsequent dots
			pos++; // start from the next character
			std::size_t next;
			while ((next = result.find('.', pos)) != std::string::npos)
			{
				result.erase(next, 1); // erase the dot
			}
		}

		return result;
	}

	string string_util::fix_number(const string& str)
	{
		string copy = str;

		std::size_t dot = copy.find(".");

		if (dot != std::string::npos)
		{
			copy = remove_all_except_first(copy, ".");

			const int dot_pos = static_cast<int>(dot);
			const int sz	  = static_cast<int>(copy.length());

			// If nothing exists after dot, insert 0.
			if (dot_pos == sz - 1) copy.insert(dot + 1, "0");

			// If nothing exists before dot insert 0.
			if (dot_pos == 0) copy.insert(0, "0");
		}

		return copy;
	}

	string string_util::from_float(float val, uint32 decimals)
	{
		char buffer[32]; // Adjust size if needed.
		auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), val, std::chars_format::fixed, decimals);
		return (ec == std::errc()) ? std::string(buffer, ptr) : ""; // Handle errors (optional).
	}

	string string_util::get_until_first_of(const string& str, const string& find)
	{
		const size_t pos = str.find_first_of(find);

		if (pos == string::npos) return "";

		return str.substr(0, str.find_first_of(str));
	}

	void string_util::split(vector<string>& out, const string& str, const string& split)
	{
		// Split the path into directories
		size_t start = 0, end = str.find(split.c_str());
		while (end != string::npos)
		{
			const auto aq = str.substr(start, end - start);
			out.push_back(aq);
			start = end + split.size();
			end	  = str.find(split.c_str(), start);
		}
		out.push_back(str.substr(start));
	}

	string string_util::to_lower(const string& input)
	{
		string data = input;

		std::for_each(data.begin(), data.end(), [](char& c) { c = ::tolower(c); });

		return data;
	}

	string string_util::to_upper(const string& input)
	{
		string data = input;

		std::for_each(data.begin(), data.end(), [](char& c) { c = ::toupper(c); });

		return data;
	}

	string string_util::remove_whitespace(const string& str)
	{
		std::string result;
		for (char c : str)
		{
			if (!std::isspace(static_cast<unsigned char>(c))) { result.push_back(c); }
		}
		return result;
	}

}

#ifdef GAME_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

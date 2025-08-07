// Copyright (c) 2025 Inan Evin

#include "string_util.hpp"
#include "memory/memory.hpp"
#include "io/assert.hpp"
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

	void string_util::replace_all(string& str, const string& to_replace, const string& replacement)
	{
		if (to_replace.empty())
			return;

		std::string result;
		result.reserve(str.size());

		size_t pos = 0;
		while (pos < str.size())
		{
			size_t found = str.find(to_replace, pos);
			if (found == std::string::npos)
			{
				result.append(str, pos, str.size() - pos);
				break;
			}
			result.append(str, pos, found - pos);
			result.append(replacement);
			pos = found + to_replace.size();
		}
		str = result;
	}

	float string_util::to_float(const string& str, uint32& outDecimals, char seperator)
	{
		try
		{
			std::size_t pos = str.find(seperator);
			if (pos != std::string::npos)
				outDecimals = static_cast<uint32>(str.length() - pos - 1);

			return std::stof(str);
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

	void string_util::append_float(float value, char* target_buffer, uint32 max_chars, uint32 decimals, bool null_term)
	{
		GAME_ASSERT(decimals < max_chars);
		GAME_ASSERT(max_chars < 16);
		int	 written = 0;
		char float_buf[16];

		for (int precision = decimals; precision >= 0; --precision)
		{
			written = snprintf(float_buf, sizeof(float_buf), "%.*f", precision, value);
			if (written <= static_cast<int>(max_chars))
				break;
		}

		GAME_MEMCPY(target_buffer, float_buf, written);

		if (null_term)
			target_buffer[written] = '\0';
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

	void string_util::to_lower(string& input)
	{
		for (char& c : input)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}

	void string_util::to_upper(string& input)
	{
		for (char& c : input)
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
	}

	void string_util::remove_whitespace(string& str)
	{
		size_t write = 0;
		for (size_t read = 0; read < str.size(); ++read)
		{
			if (!std::isspace(static_cast<unsigned char>(str[read])))
			{
				str[write++] = str[read];
			}
		}
		str.resize(write);
	}

}

#ifdef GAME_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

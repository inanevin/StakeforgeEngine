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

#include "SFG/Type/SizeDefinitions.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/Data/String.hpp"

namespace SFG
{
	class StringUtil
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="wstring"></param>
		/// <returns></returns>
		static String WideStringToString(const WString& wstring);

		/// <summary>
		///
		/// </summary>
		/// <param name="wch"></param>
		/// <returns></returns>
		static String WCharToString(wchar_t wch);

		/// <summary>
		///
		/// </summary>
		/// <param name="wch"></param>
		/// <returns></returns>
		static String EncodeUTF8(wchar_t wch);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String RemoveAllDotsExceptFirst(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String FixStringNumber(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="val"></param>
		/// <param name="decimals"></param>
		/// <returns></returns>
		static String FloatToString(float val, uint32 decimals);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <param name="toReplace"></param>
		/// <param name="replacement"></param>
		/// <returns></returns>
		static String ReplaceAll(const String& str, const String& toReplace, const String& replacement);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String GetUntilFirstOf(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String ToUpper(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String ToLower(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static String RemoveWhitespaces(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="string"></param>
		/// <returns></returns>
		static WString StringToWString(const String& string);

		/// <summary>
		///
		/// </summary>
		/// <param name="out"></param>
		/// <param name="str"></param>
		/// <param name="splitStr"></param>
		static void SplitBy(Vector<String>& out, const String& str, const String& splitStr);

		/// <summary>
		///
		/// </summary>
		/// <param name="wch"></param>
		/// <returns></returns>
		static char* WCharToChar(const wchar_t* wch);

		/// <summary>
		///
		/// </summary>
		/// <param name="ch"></param>
		/// <returns></returns>
		static const wchar_t* CharToWChar(const char* ch);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <param name="out"></param>
		/// <param name="c"></param>
		static void SeperateByChar(const String& str, Vector<String>& out, char c);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <param name="outDecimals"></param>
		/// <returns></returns>
		static float StringToFloat(const String& str, uint32& outDecimals);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static int StringToInt(const String& str);

		/// <summary>
		///
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static uint64 StringToBigInt(const String& str);
	};

} // namespace SFG

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

#ifdef SFG_PLATFORM_OSX
#include <mach/mach_time.h>
#endif

namespace SFG
{
	class Time
	{
	public:
		/// <summary>
		///
		/// </summary>
		static void Initialize();

		/// <summary>
		///
		/// </summary>
		static void Shutdown();

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static int64_t GetCPUMicroseconds();

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static int64_t GetCPUCycles();

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		static double GetCPUSeconds();

		/// <summary>
		///
		/// </summary>
		/// <param name="fromCycles"></param>
		/// <param name="toCycles"></param>
		/// <returns></returns>
		static double GetDeltaSeconds64(int64_t fromCycles, int64_t toCycles);

		/// <summary>
		///
		/// </summary>
		/// <param name="fromCycles"></param>
		/// <param name="toCycles"></param>
		/// <returns></returns>
		static int64_t GetDeltaMicroseconds64(int64_t fromCycles, int64_t toCycles);

		/// <summary>
		///
		/// </summary>
		/// <param name="microseconds"></param>
		static void Throttle(int64_t microseconds);

		/// <summary>
		///
		/// </summary>
		/// <param name="milliseconds"></param>
		static void Sleep(uint32_t milliseconds);

		/// <summary>
		///
		/// </summary>
		static void YieldThread();

	private:
#ifdef SFG_PLATFORM_OSX
		static mach_timebase_info_data_t s_timebaseInfo;
#endif

#ifdef SFG_PLATFORM_WINDOWS
		static int64_t s_frequency;
#endif
	};

} // namespace SFG

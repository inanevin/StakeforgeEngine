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

#include "SFG/Platform/Time.hpp"
#include "SFG/IO/Log.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

namespace SFG
{
	int64 Time::s_frequency = 0;

	void Time::Initialize()
	{
		if (s_frequency == 0)
		{
			timeBeginPeriod(1);

			LARGE_INTEGER frequency;

			if (!QueryPerformanceFrequency(&frequency))
			{
				SFG_ERR("[Time] -> QueryPerformanceFrequency failed!");
			}

			s_frequency = frequency.QuadPart;
		}
	}

	void Time::Shutdown()
	{
		timeEndPeriod(1);
	}

	int64 Time::GetCPUMicroseconds()
	{
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);

		// directly converting cycles to microseconds will overflow
		// first dividing with frequency will turn it into seconds and loose precision.
		return (cycles.QuadPart / s_frequency) * 1000000ll + ((cycles.QuadPart % s_frequency) * 1000000ll) / s_frequency;
	}

	double Time::GetCPUSeconds()
	{
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		return static_cast<double>(cycles.QuadPart) * 1.0 / static_cast<double>(s_frequency);
	}

	int64 Time::GetCPUCycles()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		return Cycles.QuadPart;
	}

	double Time::GetDeltaSeconds64(int64 fromCycles, int64 toCycles)
	{
		return static_cast<double>(toCycles - fromCycles) * 1.0 / (static_cast<double>(s_frequency));
	}

	int64 Time::GetDeltaMicroseconds64(int64 fromCycles, int64 toCycles)
	{
		return ((toCycles - fromCycles) * 1000000ll) / s_frequency;
	}

	void Time::Throttle(int64 microseconds)
	{
		if (microseconds < 0)
			return;

		int64		now	   = GetCPUMicroseconds();
		const int64 target = now + microseconds;
		int64		sleep  = microseconds;

		for (;;)
		{
			now = GetCPUMicroseconds();

			if (now >= target)
			{
				break;
			}

			int64 diff = target - now;

			if (diff > 2000)
			{
				uint32 ms = static_cast<uint32>((double)(diff - 2000) / 1000.0);
				Sleep(ms);
			}
			else
			{
				Sleep(0);
			}
		}
	}

	void Time::Sleep(uint32 milliseconds)
	{
		if (milliseconds == 0)
			YieldProcessor();
		else
			::Sleep(milliseconds);
	}

	void Time::YieldThread()
	{
		YieldProcessor();
	}

} // namespace SFG

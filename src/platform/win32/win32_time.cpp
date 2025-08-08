// Copyright (c) 2025 Inan Evin

#include "platform/time.hpp"
#include "io/log.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

namespace Game
{
	int64 time::s_frequency = 0;

	void time::init()
	{
		if (s_frequency == 0)
		{
			timeBeginPeriod(1);

			LARGE_INTEGER frequency;

			if (!QueryPerformanceFrequency(&frequency))
			{
				SFG_ERR("[time] -> QueryPerformanceFrequency failed!");
			}

			s_frequency = frequency.QuadPart;
		}
	}

	void time::uninit()
	{
		timeEndPeriod(1);
	}

	int64 time::get_cpu_microseconds()
	{
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);

		// directly converting cycles to microseconds will overflow
		// first dividing with frequency will turn it into seconds and loose precision.
		return (cycles.QuadPart / s_frequency) * 1000000ll + ((cycles.QuadPart % s_frequency) * 1000000ll) / s_frequency;
	}

	double time::get_cpu_seconds()
	{
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		return static_cast<double>(cycles.QuadPart) * 1.0 / static_cast<double>(s_frequency);
	}

	int64 time::get_cpu_cycles()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		return Cycles.QuadPart;
	}

	double time::get_delta_seconds(int64 fromCycles, int64 toCycles)
	{
		return static_cast<double>(toCycles - fromCycles) * 1.0 / (static_cast<double>(s_frequency));
	}

	int64 time::get_delta_microseconds(int64 fromCycles, int64 toCycles)
	{
		return ((toCycles - fromCycles) * 1000000ll) / s_frequency;
	}

	void time::throttle(int64 microseconds)
	{
		if (microseconds < 0)
			return;

		int64		now	   = get_cpu_microseconds();
		const int64 target = now + microseconds;
		int64		sleep  = microseconds;

		for (;;)
		{
			now = get_cpu_microseconds();

			if (now >= target)
			{
				break;
			}

			int64 diff = target - now;

			if (diff > 2000)
			{
				uint32 ms = static_cast<uint32>((double)(diff - 2000) / 1000.0);
				go_to_sleep(ms);
			}
			else
			{
				go_to_sleep(0);
			}
		}
	}

	void time::go_to_sleep(uint32 milliseconds)
	{
		if (milliseconds == 0)
			YieldProcessor();
		else
			::Sleep(milliseconds);
	}

	void time::YieldThread()
	{
		YieldProcessor();
	}

} // namespace SFG

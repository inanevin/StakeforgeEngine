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
#include <unistd.h>
#include <sched.h>


namespace SFG
{
	mach_timebase_info_data_t Time::s_timebaseInfo = {0, 0};

    void Time::Initialize()
    {
        mach_timebase_info(&s_timebaseInfo);
    }
    
    void Time::Shutdown()
    {
        
    }
	int64 Time::GetCPUMicroseconds()
	{
		auto time = mach_absolute_time();
		return (time * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1000);
	}

	double Time::GetCPUSeconds()
	{
		auto time = mach_absolute_time();
		return static_cast<double>((time * s_timebaseInfo.numer) / s_timebaseInfo.denom) / 1e9;
	}

	int64 Time::GetCPUCycles()
	{
		return mach_absolute_time();
	}

	double Time::GetDeltaSeconds64(int64 fromCycles, int64 toCycles)
	{
		return static_cast<double>((toCycles - fromCycles) * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1e9);
	}

	int64 Time::GetDeltaMicroseconds64(int64 fromCycles, int64 toCycles)
	{
		return ((toCycles - fromCycles) * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1000);
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
        if(milliseconds == 0)
        {
            YieldThread();
            return;
        }
        
		usleep(milliseconds * 1000);
	}

    void Time::YieldThread()
    {
        sched_yield();
    }



} // namespace SFG

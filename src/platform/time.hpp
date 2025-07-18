// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"

#ifdef GAME_PLATFORM_OSX
#include <mach/mach_time.h>
#endif

namespace Game
{
	class time
	{
	public:
		static void	  init();
		static void	  uninit();
		static int64  get_cpu_microseconds();
		static int64  get_cpu_cycles();
		static double get_cpu_seconds();
		static double get_delta_seconds(int64 fromCycles, int64 toCycles);
		static int64  get_delta_microseconds(int64 fromCycles, int64 toCycles);
		static void	  throttle(int64 microseconds);
		static void	  go_to_sleep(uint32 milliseconds);
		static void	  YieldThread();

	private:
#ifdef GAME_PLATFORM_OSX
		static mach_timebase_info_data_t s_timebaseInfo;
#endif

#ifdef GAME_PLATFORM_WINDOWS
		static int64 s_frequency;
#endif
	};

} // namespace SFG

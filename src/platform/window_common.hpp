// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "math/vector2i16.hpp"
#include "math/vector2ui16.hpp"
#include "data/bitmask.hpp"

namespace Game
{
	enum class input_code;

	struct monitor_info
	{
		vector2i16	position   = vector2i16::zero;
		vector2ui16 size	   = vector2ui16::zero;
		vector2ui16 work_size  = vector2ui16::zero;
		uint32		dpi		   = 0;
		float		dpi_scale  = 0.0f;
		bool		is_primary = false;
	};

	enum window_flags
	{
		wf_size_dirty		= 1 << 0,
		wf_has_focus		= 1 << 1,
		wf_high_freq		= 1 << 2,
		wf_close_requested	= 1 << 3,
		wf_style_windowed	= 1 << 4,
		wf_style_borderless = 1 << 5,
	};

	enum window_event_flags
	{
		wef_high_freq = 1 << 0,
	};

	enum class window_event_type : uint8
	{
		key = 0,
		mouse,
		wheel,
		delta,
	};

	enum class window_event_sub_type : uint8
	{
		press,
		release,
		repeat,
	};

	struct window_event
	{
		vector2i16			  value = vector2i16::zero;
		input_code			  button;
		window_event_type	  type	   = window_event_type::key;
		window_event_sub_type sub_type = window_event_sub_type::press;
		bitmask<uint8>		  flags	   = 0;
	};
}
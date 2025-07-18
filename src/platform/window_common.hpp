// Copyright (c) 2025 Inan Evin

#include "common/size_definitions.hpp"
#include "math/vector2i.hpp"
#include "math/vector2ui.hpp"
#include "data/bitmask.hpp"

namespace Game
{
	enum class input_code;

	struct monitor_info
	{
		vector2i  position	 = vector2i();
		vector2ui size		 = vector2ui();
		vector2ui work_size	 = vector2ui();
		uint32	  dpi		 = 0;
		float	  dpi_scale	 = 0.0f;
		bool	  is_primary = false;
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

	enum class window_event_type
	{
		key,
		mouse_button,
		mouse_wheel,
		mouse_delta,
	};

	enum window_event_flags
	{
		wef_is_key	   = 1 << 0,
		wef_is_mouse   = 1 << 1,
		wef_is_wheel   = 1 << 2,
		wef_is_press   = 1 << 3,
		wef_is_repeat  = 1 << 4,
		wef_is_release = 1 << 5,
		wef_is_delta   = 1 << 6,
		wef_high_freq  = 1 << 7,
	};

	struct window_event
	{
		vector2i	   value = vector2i();
		bitmask<uint8> flags = 0;
		input_code	   button;
	};
}
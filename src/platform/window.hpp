// Copyright (c) 2025 Inan Evin
#pragma once

#include "window_common.hpp"

#ifdef GAME_PLATFORM_WINDOWS
struct HWND__;
#endif
namespace Game
{
#define MAX_EVENTS 32

	class window
	{
	public:
		bool create(const char* title, uint8 flags, const vector2i& pos, const vector2ui& size);
		void destroy();
		void set_position(const vector2i& pos);
		void set_size(const vector2i& size);
		void set_style(window_flags flags);
		void bring_to_front();
		void add_event(const window_event& ev);

		inline const vector2i get_position() const
		{
			return _position;
		}
		inline const vector2ui get_size() const
		{
			return _size;
		}
		inline void* get_window_handle() const
		{
			return _window_handle;
		}
		inline void* get_platform_handle() const
		{
			return _platform_handle;
		}
		inline window_event* get_events()
		{
			return _events;
		}
		inline uint32 get_event_count() const
		{
			return _event_count;
		}
		inline void clear_events()
		{
			_event_count = 0;
		}

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

#ifdef GAME_PLATFORM_WINDOWS
		static __int64 wnd_proc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
#endif

	private:
		window_event   _events[MAX_EVENTS];
		monitor_info   _monitor_info	   = {};
		void*		   _window_handle	   = nullptr;
		void*		   _platform_handle	   = nullptr;
		vector2i	   _mouse_position	   = vector2i();
		vector2i	   _mouse_position_abs = vector2i();
		vector2i	   _position		   = vector2i();
		vector2ui	   _true_size		   = vector2ui();
		vector2ui	   _size			   = vector2ui();
		uint32		   _event_count		   = 0;
		bitmask<uint8> _flags			   = 0;
	};

	/*
	struct window_meta
	{
		void*		   handle		   = nullptr;
		void*		   platform_handle = nullptr;
		bitmask<uint8> flags;
	};

	struct window_mouse_data
	{
		vector2i position	  = vector2i::zero;
		vector2i position_abs = vector2i::zero;
	};

	struct window_layout_data
	{
		vector2i  position = vector2i::zero;
		vector2ui size	   = vector2ui::zero;
	};
	*/
}
// Copyright (c) 2025 Inan Evin
#pragma once

#include "window_common.hpp"
#include "data/hash_map.hpp"
#include "memory/malloc_allocator_map.hpp"

#ifdef SFG_PLATFORM_WINDOWS
struct HWND__;
#endif
namespace SFG
{
#define MAX_EVENTS 32

	class window
	{
	private:
		typedef phmap::flat_hash_map<uint16, uint8, phmap::priv::hash_default_hash<uint16>, phmap::priv::hash_default_eq<uint16>, malloc_allocator_map<uint16>> map;

	public:
		bool create(const char* title, uint8 flags, const vector2i16& pos, const vector2ui16& size);
		void destroy();
		void set_position(const vector2i16& pos);
		void set_size(const vector2ui16& size);
		void set_style(window_flags flags);
		void bring_to_front();
		void add_event(const window_event& ev);

		inline const vector2i16 get_position() const
		{
			return _position;
		}
		inline const vector2ui16 get_size() const
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

		inline const monitor_info& get_monitor_info() const
		{
			return _monitor_info;
		}

#ifdef SFG_PLATFORM_WINDOWS
		static __int64 wnd_proc(HWND__* hwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
#endif

	private:
		window_event   _events[MAX_EVENTS];
		monitor_info   _monitor_info	   = {};
		void*		   _window_handle	   = nullptr;
		void*		   _platform_handle	   = nullptr;
		vector2i16	   _mouse_position	   = vector2i16::zero;
		vector2i16	   _mouse_position_abs = vector2i16::zero;
		vector2i16	   _position		   = vector2i16::zero;
		vector2ui16	   _true_size		   = vector2ui16::zero;
		vector2ui16	   _size			   = vector2ui16::zero;
		uint32		   _event_count		   = 0;
		bitmask<uint8> _flags			   = 0;
		static map	   s_key_down_map;
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
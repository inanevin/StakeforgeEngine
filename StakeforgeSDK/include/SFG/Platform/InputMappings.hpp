/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2025-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

#ifdef SFG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

namespace SFG
{

#ifdef SFG_PLATFORM_WINDOWS

#define SFG_MOUSE_0		 VK_LBUTTON
#define SFG_MOUSE_1		 VK_RBUTTON
#define SFG_MOUSE_2		 VK_MBUTTON
#define SFG_MOUSE_3		 VK_XBUTTON1
#define SFG_MOUSE_4		 VK_XBUTTON2
#define SFG_MOUSE_5		 VK_LBUTTON
#define SFG_MOUSE_6		 VK_LBUTTON
#define SFG_MOUSE_7		 VK_LBUTTON
#define SFG_MOUSE_LAST	 SFG_MOUSE_7
#define SFG_MOUSE_LEFT	 SFG_MOUSE_0
#define SFG_MOUSE_RIGHT	 SFG_MOUSE_1
#define SFG_MOUSE_MIDDLE SFG_MOUSE_2

#define SFG_GAMEPAD_A			  VK_PAD_A
#define SFG_GAMEPAD_B			  VK_PAD_B
#define SFG_GAMEPAD_X			  VK_PAD_X
#define SFG_GAMEPAD_Y			  VK_PAD_Y
#define SFG_GAMEPAD_LEFT_BUMPER	  VK_PAD_LSHOULDER
#define SFG_GAMEPAD_RIGHT_BUMPER  VK_PAD_RSHOULDER
#define SFG_GAMEPAD_BACK		  VK_PAD_BACK
#define SFG_GAMEPAD_START		  VK_PAD_START
#define SFG_GAMEPAD_GUIDE		  VK_PAD_START
#define SFG_GAMEPAD_LEFT_THUMB	  VK_PAD_LTHUMB_PRESS
#define SFG_GAMEPAD_RIGHT_THUMB	  VK_PAD_RTHUMB_PRESS
#define SFG_GAMEPAD_DPAD_UP		  VK_PAD_DPAD_UP
#define SFG_GAMEPAD_DPAD_RIGHT	  VK_PAD_DPAD_RIGHT
#define SFG_GAMEPAD_DPAD_DOWN	  VK_PAD_DPAD_DOWN
#define SFG_GAMEPAD_DPAD_LEFT	  VK_PAD_DPAD_LEFT
#define SFG_GAMEPAD_LEFT_TRIGGER  4
#define SFG_GAMEPAD_RIGHT_TRIGGER 5
#define SFG_GAMEPAD_LAST		  SFG_GAMEPAD_RIGHT_TRIGGER

#define SFG_KEY_UNKNOWN -1
#define SFG_KEY_A		0x41
#define SFG_KEY_B		0x42
#define SFG_KEY_C		0x43
#define SFG_KEY_D		0x44
#define SFG_KEY_E		0x45
#define SFG_KEY_F		0x46
#define SFG_KEY_G		0x47
#define SFG_KEY_H		0x48
#define SFG_KEY_I		0x49
#define SFG_KEY_J		0x4A
#define SFG_KEY_K		0x4B
#define SFG_KEY_L		0x4C
#define SFG_KEY_M		0x4D
#define SFG_KEY_N		0x4E
#define SFG_KEY_O		0x4F
#define SFG_KEY_P		0x50
#define SFG_KEY_Q		0x51
#define SFG_KEY_R		0x52
#define SFG_KEY_S		0x53
#define SFG_KEY_T		0x54
#define SFG_KEY_U		0x55
#define SFG_KEY_V		0x56
#define SFG_KEY_W		0x57
#define SFG_KEY_X		0x58
#define SFG_KEY_Y		0x59
#define SFG_KEY_Z		0x5A

#define SFG_KEY_0 0x30
#define SFG_KEY_1 0x31
#define SFG_KEY_2 0x32
#define SFG_KEY_3 0x33
#define SFG_KEY_4 0x34
#define SFG_KEY_5 0x35
#define SFG_KEY_6 0x36
#define SFG_KEY_7 0x37
#define SFG_KEY_8 0x38
#define SFG_KEY_9 0x39

#define SFG_KEY_RETURN	  VK_RETURN
#define SFG_KEY_ESCAPE	  VK_ESCAPE
#define SFG_KEY_BACKSPACE VK_BACK
#define SFG_KEY_TAB		  VK_TAB
#define SFG_KEY_SPACE	  VK_SPACE

#define SFG_KEY_MINUS	 VK_OEM_MINUS
#define SFG_KEY_GRAVE	 VK_OEM_3 // 41 // ?
#define SFG_KEY_COMMA	 VK_OEM_COMMA
#define SFG_KEY_PERIOD	 VK_OEM_PERIOD
#define SFG_KEY_SLASH	 VK_DIVIDE
#define SFG_KEY_CAPSLOCK VK_CAPITAL

#define SFG_KEY_F1	VK_F1
#define SFG_KEY_F2	VK_F2
#define SFG_KEY_F3	VK_F3
#define SFG_KEY_F4	VK_F4
#define SFG_KEY_F5	VK_F5
#define SFG_KEY_F6	VK_F6
#define SFG_KEY_F7	VK_F7
#define SFG_KEY_F8	VK_F8
#define SFG_KEY_F9	VK_F9
#define SFG_KEY_F10 VK_F10
#define SFG_KEY_F11 VK_F11
#define SFG_KEY_F12 VK_F12
#define SFG_KEY_F13 VK_F13
#define SFG_KEY_F14 VK_F14
#define SFG_KEY_F15 VK_F15

#define SFG_KEY_PRINTSCREEN	 VK_PRINT
#define SFG_KEY_SCROLLLOCK	 VK_SCROLL
#define SFG_KEY_PAUSE		 VK_PAUSE
#define SFG_KEY_INSERT		 VK_INSERT
#define SFG_KEY_HOME		 VK_HOME
#define SFG_KEY_PAGEUP		 VK_PRIOR
#define SFG_KEY_DELETE		 VK_DELETE
#define SFG_KEY_END			 VK_END
#define SFG_KEY_PAGEDOWN	 VK_NEXT
#define SFG_KEY_RIGHT		 VK_RIGHT
#define SFG_KEY_LEFT		 VK_LEFT
#define SFG_KEY_DOWN		 VK_DOWN
#define SFG_KEY_UP			 VK_UP
#define SFG_KEY_NUMLOCKCLEAR VK_CLEAR

#define SFG_KEY_KP_DECIMAL	VK_DECIMAL
#define SFG_KEY_KP_DIVIDE	VK_DIVIDE
#define SFG_KEY_KP_MULTIPLY VK_MULTIPLY
#define SFG_KEY_KP_MINUS	VK_OEM_MINUS
#define SFG_KEY_KP_PLUS		VK_OEM_PLUS
#define SFG_KEY_KP_ENTER	VK_RETURN
#define SFG_KEY_KP_1		VK_NUMPAD1
#define SFG_KEY_KP_2		VK_NUMPAD2
#define SFG_KEY_KP_3		VK_NUMPAD3
#define SFG_KEY_KP_4		VK_NUMPAD4
#define SFG_KEY_KP_5		VK_NUMPAD5
#define SFG_KEY_KP_6		VK_NUMPAD6
#define SFG_KEY_KP_7		VK_NUMPAD7
#define SFG_KEY_KP_8		VK_NUMPAD8
#define SFG_KEY_KP_9		VK_NUMPAD9
#define SFG_KEY_KP_0		VK_NUMPAD0

#define SFG_KEY_LCTRL  VK_LCONTROL
#define SFG_KEY_LSHIFT VK_LSHIFT
#define SFG_KEY_LALT   VK_LMENU
#define SFG_KEY_LGUI   VK_LMENU
#define SFG_KEY_RCTRL  VK_RCONTROL
#define SFG_KEY_RSHIFT VK_RSHIFT
#define SFG_KEY_RALT   VK_RMENU
#define SFG_KEY_RGUI   VK_RMENU

#endif

#ifdef SFG_PLATFORM_APPLE

#define SFG_MOUSE_0		 0
#define SFG_MOUSE_1		 1
#define SFG_MOUSE_2		 2
#define SFG_MOUSE_3		 3
#define SFG_MOUSE_4		 4
#define SFG_MOUSE_5		 5
#define SFG_MOUSE_6		 6
#define SFG_MOUSE_7		 7
#define SFG_MOUSE_LAST	 SFG_MOUSE_7
#define SFG_MOUSE_LEFT	 SFG_MOUSE_0
#define SFG_MOUSE_RIGHT	 SFG_MOUSE_1
#define SFG_MOUSE_MIDDLE SFG_MOUSE_2

#define SFG_GAMEPAD_A			  0
#define SFG_GAMEPAD_B			  1
#define SFG_GAMEPAD_X			  2
#define SFG_GAMEPAD_Y			  3
#define SFG_GAMEPAD_LEFT_BUMPER	  4
#define SFG_GAMEPAD_RIGHT_BUMPER  5
#define SFG_GAMEPAD_BACK		  6
#define SFG_GAMEPAD_START		  7
#define SFG_GAMEPAD_GUIDE		  8
#define SFG_GAMEPAD_LEFT_THUMB	  9
#define SFG_GAMEPAD_RIGHT_THUMB	  10
#define SFG_GAMEPAD_DPAD_UP		  11
#define SFG_GAMEPAD_DPAD_RIGHT	  12
#define SFG_GAMEPAD_DPAD_DOWN	  13
#define SFG_GAMEPAD_DPAD_LEFT	  14
#define SFG_GAMEPAD_LEFT_TRIGGER  4
#define SFG_GAMEPAD_RIGHT_TRIGGER 5
#define SFG_GAMEPAD_LAST		  SFG_GAMEPAD_RIGHT_TRIGGER

#define SFG_KEY_UNKNOWN -1
#define SFG_KEY_A		0x00
#define SFG_KEY_B		0x0B
#define SFG_KEY_C		0x08
#define SFG_KEY_D		0x02
#define SFG_KEY_E		0x0E
#define SFG_KEY_F		0x03
#define SFG_KEY_G		0x05
#define SFG_KEY_H		0x04
#define SFG_KEY_I		0x22
#define SFG_KEY_J		0x26
#define SFG_KEY_K		0x28
#define SFG_KEY_L		0x25
#define SFG_KEY_M		0x2E
#define SFG_KEY_N		0x2D
#define SFG_KEY_O		0x1F
#define SFG_KEY_P		0x23
#define SFG_KEY_Q		0x0C
#define SFG_KEY_R		0x0F
#define SFG_KEY_S		0x01
#define SFG_KEY_T		0x11
#define SFG_KEY_U		0x20
#define SFG_KEY_V		0x09
#define SFG_KEY_W		0x0D
#define SFG_KEY_X		0x07
#define SFG_KEY_Y		0x10
#define SFG_KEY_Z		0x06

#define SFG_KEY_0 0x1D
#define SFG_KEY_1 0x12
#define SFG_KEY_2 0x13
#define SFG_KEY_3 0x14
#define SFG_KEY_4 0x15
#define SFG_KEY_5 0x17
#define SFG_KEY_6 0x16
#define SFG_KEY_7 0x1A
#define SFG_KEY_8 0x1C
#define SFG_KEY_9 0x19

#define SFG_KEY_RETURN	  0x24
#define SFG_KEY_ESCAPE	  0x35
#define SFG_KEY_BACKSPACE 0x33
#define SFG_KEY_TAB		  0x30
#define SFG_KEY_SPACE	  0x31

#define SFG_KEY_MINUS	 0x1B // '-'
#define SFG_KEY_GRAVE	 0x32 // '`'
#define SFG_KEY_COMMA	 0x2B // ','
#define SFG_KEY_PERIOD	 0x2F // '.'
#define SFG_KEY_SLASH	 0x2C // '/'
#define SFG_KEY_CAPSLOCK 0x39

#define SFG_KEY_F1	0x7A
#define SFG_KEY_F2	0x78
#define SFG_KEY_F3	0x63
#define SFG_KEY_F4	0x76
#define SFG_KEY_F5	0x60
#define SFG_KEY_F6	0x61
#define SFG_KEY_F7	0x62
#define SFG_KEY_F8	0x64
#define SFG_KEY_F9	0x65
#define SFG_KEY_F10 0x6D
#define SFG_KEY_F11 0x67
#define SFG_KEY_F12 0x6F
#define SFG_KEY_F13 0x69
#define SFG_KEY_F14 0x6B
#define SFG_KEY_F15 0x71

#define SFG_KEY_PRINTSCREEN SFG_KEY_UNKNOWN
#define SFG_KEY_SCROLLLOCK	SFG_KEY_UNKNOWN
#define SFG_KEY_PAUSE		SFG_KEY_UNKNOWN

#define SFG_KEY_INSERT		 0x72 // "Help" key on some older Mac keyboards
#define SFG_KEY_HOME		 0x73
#define SFG_KEY_PAGEUP		 0x74
#define SFG_KEY_DELETE		 0x75
#define SFG_KEY_END			 0x77
#define SFG_KEY_PAGEDOWN	 0x79
#define SFG_KEY_RIGHT		 0x7C
#define SFG_KEY_LEFT		 0x7B
#define SFG_KEY_DOWN		 0x7D
#define SFG_KEY_UP			 0x7E
#define SFG_KEY_NUMLOCKCLEAR SFG_KEY_UNKNOWN

#define SFG_KEY_KP_DECIMAL	0x41
#define SFG_KEY_KP_DIVIDE	0x4B
#define SFG_KEY_KP_MULTIPLY 0x43
#define SFG_KEY_KP_MINUS	0x4E
#define SFG_KEY_KP_PLUS		0x45
#define SFG_KEY_KP_ENTER	0x4C
#define SFG_KEY_KP_1		0x53
#define SFG_KEY_KP_2		0x54
#define SFG_KEY_KP_3		0x55
#define SFG_KEY_KP_4		0x56
#define SFG_KEY_KP_5		0x57
#define SFG_KEY_KP_6		0x58
#define SFG_KEY_KP_7		0x59
#define SFG_KEY_KP_8		0x5B
#define SFG_KEY_KP_9		0x5C
#define SFG_KEY_KP_0		0x52

#define SFG_KEY_LCTRL  0x3B
#define SFG_KEY_LSHIFT 0x38
#define SFG_KEY_LALT   0x3A // Option key
#define SFG_KEY_LGUI   0x37 // Command key
#define SFG_KEY_RCTRL  0x3E
#define SFG_KEY_RSHIFT 0x3C
#define SFG_KEY_RALT   0x3D // Option key
#define SFG_KEY_RGUI   0x36 // Command key

#endif

	enum InputCode
	{
		KeyUnknown		  = SFG_KEY_UNKNOWN,
		KeyA			  = SFG_KEY_A,
		KeyB			  = SFG_KEY_B,
		KeyC			  = SFG_KEY_C,
		KeyD			  = SFG_KEY_D,
		KeyE			  = SFG_KEY_E,
		KeyF			  = SFG_KEY_F,
		KeyG			  = SFG_KEY_G,
		KeyH			  = SFG_KEY_H,
		KeyI			  = SFG_KEY_I,
		KeyJ			  = SFG_KEY_J,
		KeyK			  = SFG_KEY_K,
		KeyL			  = SFG_KEY_L,
		KeyM			  = SFG_KEY_M,
		KeyN			  = SFG_KEY_N,
		KeyO			  = SFG_KEY_O,
		KeyP			  = SFG_KEY_P,
		KeyQ			  = SFG_KEY_Q,
		KeyR			  = SFG_KEY_R,
		KeyS			  = SFG_KEY_S,
		KeyT			  = SFG_KEY_T,
		KeyU			  = SFG_KEY_U,
		KeyV			  = SFG_KEY_V,
		KeyW			  = SFG_KEY_W,
		KeyX			  = SFG_KEY_X,
		KeyY			  = SFG_KEY_Y,
		KeyZ			  = SFG_KEY_Z,
		KeyAlpha0		  = SFG_KEY_0,
		KeyAlpha1		  = SFG_KEY_1,
		KeyAlpha2		  = SFG_KEY_2,
		KeyAlpha3		  = SFG_KEY_3,
		KeyAlpha4		  = SFG_KEY_4,
		KeyAlpha5		  = SFG_KEY_5,
		KeyAlpha6		  = SFG_KEY_6,
		KeyAlpha7		  = SFG_KEY_7,
		KeyAlpha8		  = SFG_KEY_8,
		KeyAlpha9		  = SFG_KEY_9,
		KeyReturn		  = SFG_KEY_RETURN,
		KeyEscape		  = SFG_KEY_ESCAPE,
		KeyBackspace	  = SFG_KEY_BACKSPACE,
		KeyTab			  = SFG_KEY_TAB,
		KeySpace		  = SFG_KEY_SPACE,
		KeyMinus		  = SFG_KEY_MINUS,
		KeyGrave		  = SFG_KEY_GRAVE,
		KeyComma		  = SFG_KEY_COMMA,
		KeyPeriod		  = SFG_KEY_PERIOD,
		KeySlash		  = SFG_KEY_SLASH,
		KeyCapslock		  = SFG_KEY_CAPSLOCK,
		KeyF1			  = SFG_KEY_F1,
		KeyF2			  = SFG_KEY_F2,
		KeyF3			  = SFG_KEY_F3,
		KeyF4			  = SFG_KEY_F4,
		KeyF5			  = SFG_KEY_F5,
		KeyF6			  = SFG_KEY_F6,
		KeyF7			  = SFG_KEY_F7,
		KeyF8			  = SFG_KEY_F8,
		KeyF9			  = SFG_KEY_F9,
		KeyF10			  = SFG_KEY_F10,
		KeyF11			  = SFG_KEY_F11,
		KeyF12			  = SFG_KEY_F12,
		KeyPrintscreen	  = SFG_KEY_PRINTSCREEN,
		KeyScrolllock	  = SFG_KEY_SCROLLLOCK,
		KeyPause		  = SFG_KEY_PAUSE,
		KeyInsert		  = SFG_KEY_INSERT,
		KeyHome			  = SFG_KEY_HOME,
		KeyPageup		  = SFG_KEY_PAGEUP,
		KeyDelete		  = SFG_KEY_DELETE,
		KeyEnd			  = SFG_KEY_END,
		KeyPagedown		  = SFG_KEY_PAGEDOWN,
		KeyRight		  = SFG_KEY_RIGHT,
		KeyLeft			  = SFG_KEY_LEFT,
		KeyDown			  = SFG_KEY_DOWN,
		KeyUp			  = SFG_KEY_UP,
		KeyNumlockClear	  = SFG_KEY_NUMLOCKCLEAR,
		KeyKeypadDecimal  = SFG_KEY_KP_DECIMAL,
		KeyKeypadDivide	  = SFG_KEY_KP_DIVIDE,
		KeyKeypadMultiply = SFG_KEY_KP_MULTIPLY,
		KeyKeypadMinus	  = SFG_KEY_KP_MINUS,
		KeyKeypadPlus	  = SFG_KEY_KP_PLUS,
		KeyKeypadEnter	  = SFG_KEY_KP_ENTER,
		KeyKeypad1		  = SFG_KEY_KP_1,
		KeyKeypad2		  = SFG_KEY_KP_2,
		KeyKeypad3		  = SFG_KEY_KP_3,
		KeyKeypad4		  = SFG_KEY_KP_4,
		KeyKeypad5		  = SFG_KEY_KP_5,
		KeyKeypad6		  = SFG_KEY_KP_6,
		KeyKeypad7		  = SFG_KEY_KP_7,
		KeyKeypad8		  = SFG_KEY_KP_8,
		KeyKeypad9		  = SFG_KEY_KP_9,
		KeyKeypad0		  = SFG_KEY_KP_0,
		KeyF13			  = SFG_KEY_F13,
		KeyF14			  = SFG_KEY_F14,
		KeyF15			  = SFG_KEY_F15,
		KeyLCTRL		  = SFG_KEY_LCTRL,
		KeyLSHIFT		  = SFG_KEY_LSHIFT,
		KeyLALT			  = SFG_KEY_LALT,
		KeyLGUI			  = SFG_KEY_LGUI,
		KeyRCTRL		  = SFG_KEY_RCTRL,
		KeyRSHIFT		  = SFG_KEY_RSHIFT,
		KeyRALT			  = SFG_KEY_RALT,
		KeyRGUI			  = SFG_KEY_RGUI,

		MouseUnknown	= SFG_KEY_UNKNOWN,
		Mouse0			= SFG_MOUSE_0,
		Mouse1			= SFG_MOUSE_1,
		Mouse2			= SFG_MOUSE_2,
		Mouse3			= SFG_MOUSE_3,
		Mouse4			= SFG_MOUSE_4,
		Mouse5			= SFG_MOUSE_5,
		Mouse6			= SFG_MOUSE_6,
		Mouse7			= SFG_MOUSE_7,
		MouseLast		= SFG_MOUSE_LAST,
		MouseLeft		= SFG_MOUSE_LEFT,
		MouseRight		= SFG_MOUSE_RIGHT,
		MouseMiddle		= SFG_MOUSE_MIDDLE,
		JoystickUnknown = SFG_KEY_UNKNOWN,

		GamepadUnknown	 = SFG_KEY_UNKNOWN,
		GamepadA		 = SFG_GAMEPAD_A,
		GamepadB		 = SFG_GAMEPAD_B,
		GamepadX		 = SFG_GAMEPAD_X,
		GamepadY		 = SFG_GAMEPAD_Y,
		GamepadLBumper	 = SFG_GAMEPAD_LEFT_BUMPER,
		GamepadRBumper	 = SFG_GAMEPAD_RIGHT_BUMPER,
		GamepadBack		 = SFG_GAMEPAD_BACK,
		GamepadStart	 = SFG_GAMEPAD_START,
		GamepadGuide	 = SFG_GAMEPAD_GUIDE,
		GamepadLThumb	 = SFG_GAMEPAD_LEFT_THUMB,
		GamepadRThumb	 = SFG_GAMEPAD_RIGHT_THUMB,
		GamepadDpadUp	 = SFG_GAMEPAD_DPAD_UP,
		GamepadDpadRight = SFG_GAMEPAD_DPAD_RIGHT,
		GamepadDpadDown	 = SFG_GAMEPAD_DPAD_DOWN,
		GamepadDpadLeft	 = SFG_GAMEPAD_DPAD_LEFT,
		GamepadDpadLast	 = SFG_GAMEPAD_LAST,
		GamepadLTrigger	 = SFG_GAMEPAD_LEFT_TRIGGER,
		GamepadRTrigger	 = SFG_GAMEPAD_RIGHT_TRIGGER,
		GamepadLast		 = SFG_GAMEPAD_LAST
	};

} // namespace SFG

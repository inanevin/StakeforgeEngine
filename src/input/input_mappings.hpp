// Copyright (c) 2025 Inan Evin

#pragma once

#ifdef GAME_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

namespace Game
{

#ifdef GAME_PLATFORM_WINDOWS

#define GAME_MOUSE_0	  VK_LBUTTON
#define GAME_MOUSE_1	  VK_RBUTTON
#define GAME_MOUSE_2	  VK_MBUTTON
#define GAME_MOUSE_3	  VK_XBUTTON1
#define GAME_MOUSE_4	  VK_XBUTTON2
#define GAME_MOUSE_5	  VK_LBUTTON
#define GAME_MOUSE_6	  VK_LBUTTON
#define GAME_MOUSE_7	  VK_LBUTTON
#define GAME_MOUSE_LAST	  GAME_MOUSE_7
#define GAME_MOUSE_LEFT	  GAME_MOUSE_0
#define GAME_MOUSE_RIGHT  GAME_MOUSE_1
#define GAME_MOUSE_MIDDLE GAME_MOUSE_2

#define GAME_GAMEPAD_A			   VK_PAD_A
#define GAME_GAMEPAD_B			   VK_PAD_B
#define GAME_GAMEPAD_X			   VK_PAD_X
#define GAME_GAMEPAD_Y			   VK_PAD_Y
#define GAME_GAMEPAD_LEFT_BUMPER   VK_PAD_LSHOULDER
#define GAME_GAMEPAD_RIGHT_BUMPER  VK_PAD_RSHOULDER
#define GAME_GAMEPAD_BACK		   VK_PAD_BACK
#define GAME_GAMEPAD_START		   VK_PAD_START
#define GAME_GAMEPAD_GUIDE		   VK_PAD_START
#define GAME_GAMEPAD_LEFT_THUMB	   VK_PAD_LTHUMB_PRESS
#define GAME_GAMEPAD_RIGHT_THUMB   VK_PAD_RTHUMB_PRESS
#define GAME_GAMEPAD_DPAD_UP	   VK_PAD_DPAD_UP
#define GAME_GAMEPAD_DPAD_RIGHT	   VK_PAD_DPAD_RIGHT
#define GAME_GAMEPAD_DPAD_DOWN	   VK_PAD_DPAD_DOWN
#define GAME_GAMEPAD_DPAD_LEFT	   VK_PAD_DPAD_LEFT
#define GAME_GAMEPAD_LEFT_TRIGGER  4
#define GAME_GAMEPAD_RIGHT_TRIGGER 5
#define GAME_GAMEPAD_LAST		   GAME_GAMEPAD_RIGHT_TRIGGER

#define GAME_KEY_UNKNOWN -1
#define GAME_KEY_A		 0x41
#define GAME_KEY_B		 0x42
#define GAME_KEY_C		 0x43
#define GAME_KEY_D		 0x44
#define GAME_KEY_E		 0x45
#define GAME_KEY_F		 0x46
#define GAME_KEY_G		 0x47
#define GAME_KEY_H		 0x48
#define GAME_KEY_I		 0x49
#define GAME_KEY_J		 0x4A
#define GAME_KEY_K		 0x4B
#define GAME_KEY_L		 0x4C
#define GAME_KEY_M		 0x4D
#define GAME_KEY_N		 0x4E
#define GAME_KEY_O		 0x4F
#define GAME_KEY_P		 0x50
#define GAME_KEY_Q		 0x51
#define GAME_KEY_R		 0x52
#define GAME_KEY_S		 0x53
#define GAME_KEY_T		 0x54
#define GAME_KEY_U		 0x55
#define GAME_KEY_V		 0x56
#define GAME_KEY_W		 0x57
#define GAME_KEY_X		 0x58
#define GAME_KEY_Y		 0x59
#define GAME_KEY_Z		 0x5A

#define GAME_KEY_0 0x30
#define GAME_KEY_1 0x31
#define GAME_KEY_2 0x32
#define GAME_KEY_3 0x33
#define GAME_KEY_4 0x34
#define GAME_KEY_5 0x35
#define GAME_KEY_6 0x36
#define GAME_KEY_7 0x37
#define GAME_KEY_8 0x38
#define GAME_KEY_9 0x39

#define GAME_KEY_RETURN	   VK_RETURN
#define GAME_KEY_ESCAPE	   VK_ESCAPE
#define GAME_KEY_BACKSPACE VK_BACK
#define GAME_KEY_TAB	   VK_TAB
#define GAME_KEY_SPACE	   VK_SPACE

#define GAME_KEY_MINUS		   VK_OEM_MINUS
#define GAME_KEY_TILDE		   VK_OEM_3 // 41 // ?
#define GAME_KEY_ANGLE_BRACKET VK_OEM_102
#define GAME_KEY_COMMA		   VK_OEM_COMMA
#define GAME_KEY_PERIOD		   VK_OEM_PERIOD
#define GAME_KEY_SLASH		   VK_DIVIDE
#define GAME_KEY_CAPSLOCK	   VK_CAPITAL

#define GAME_KEY_F1	 VK_F1
#define GAME_KEY_F2	 VK_F2
#define GAME_KEY_F3	 VK_F3
#define GAME_KEY_F4	 VK_F4
#define GAME_KEY_F5	 VK_F5
#define GAME_KEY_F6	 VK_F6
#define GAME_KEY_F7	 VK_F7
#define GAME_KEY_F8	 VK_F8
#define GAME_KEY_F9	 VK_F9
#define GAME_KEY_F10 VK_F10
#define GAME_KEY_F11 VK_F11
#define GAME_KEY_F12 VK_F12
#define GAME_KEY_F13 VK_F13
#define GAME_KEY_F14 VK_F14
#define GAME_KEY_F15 VK_F15

#define GAME_KEY_PRINTSCREEN  VK_PRINT
#define GAME_KEY_SCROLLLOCK	  VK_SCROLL
#define GAME_KEY_PAUSE		  VK_PAUSE
#define GAME_KEY_INSERT		  VK_INSERT
#define GAME_KEY_HOME		  VK_HOME
#define GAME_KEY_PAGEUP		  VK_PRIOR
#define GAME_KEY_DELETE		  VK_DELETE
#define GAME_KEY_END		  VK_END
#define GAME_KEY_PAGEDOWN	  VK_NEXT
#define GAME_KEY_RIGHT		  VK_RIGHT
#define GAME_KEY_LEFT		  VK_LEFT
#define GAME_KEY_DOWN		  VK_DOWN
#define GAME_KEY_UP			  VK_UP
#define GAME_KEY_NUMLOCKCLEAR VK_CLEAR

#define GAME_KEY_KP_DECIMAL	 VK_DECIMAL
#define GAME_KEY_KP_DIVIDE	 VK_DIVIDE
#define GAME_KEY_KP_MULTIPLY VK_MULTIPLY
#define GAME_KEY_KP_MINUS	 VK_OEM_MINUS
#define GAME_KEY_KP_PLUS	 VK_OEM_PLUS
#define GAME_KEY_KP_ENTER	 VK_RETURN
#define GAME_KEY_KP_1		 VK_NUMPAD1
#define GAME_KEY_KP_2		 VK_NUMPAD2
#define GAME_KEY_KP_3		 VK_NUMPAD3
#define GAME_KEY_KP_4		 VK_NUMPAD4
#define GAME_KEY_KP_5		 VK_NUMPAD5
#define GAME_KEY_KP_6		 VK_NUMPAD6
#define GAME_KEY_KP_7		 VK_NUMPAD7
#define GAME_KEY_KP_8		 VK_NUMPAD8
#define GAME_KEY_KP_9		 VK_NUMPAD9
#define GAME_KEY_KP_0		 VK_NUMPAD0

#define GAME_KEY_LCTRL	VK_LCONTROL
#define GAME_KEY_LSHIFT VK_LSHIFT
#define GAME_KEY_LALT	VK_LMENU
#define GAME_KEY_LGUI	VK_LMENU
#define GAME_KEY_RCTRL	VK_RCONTROL
#define GAME_KEY_RSHIFT VK_RSHIFT
#define GAME_KEY_RALT	VK_RMENU
#define GAME_KEY_RGUI	VK_RMENU

#endif

#ifdef GAME_PLATFORM_OSX

#define GAME_MOUSE_0	  0
#define GAME_MOUSE_1	  1
#define GAME_MOUSE_2	  2
#define GAME_MOUSE_3	  3
#define GAME_MOUSE_4	  4
#define GAME_MOUSE_5	  5
#define GAME_MOUSE_6	  6
#define GAME_MOUSE_7	  7
#define GAME_MOUSE_LAST	  GAME_MOUSE_7
#define GAME_MOUSE_LEFT	  GAME_MOUSE_0
#define GAME_MOUSE_RIGHT  GAME_MOUSE_1
#define GAME_MOUSE_MIDDLE GAME_MOUSE_2

#define GAME_GAMEPAD_A			   0
#define GAME_GAMEPAD_B			   1
#define GAME_GAMEPAD_X			   2
#define GAME_GAMEPAD_Y			   3
#define GAME_GAMEPAD_LEFT_BUMPER   4
#define GAME_GAMEPAD_RIGHT_BUMPER  5
#define GAME_GAMEPAD_BACK		   6
#define GAME_GAMEPAD_START		   7
#define GAME_GAMEPAD_GUIDE		   8
#define GAME_GAMEPAD_LEFT_THUMB	   9
#define GAME_GAMEPAD_RIGHT_THUMB   10
#define GAME_GAMEPAD_DPAD_UP	   11
#define GAME_GAMEPAD_DPAD_RIGHT	   12
#define GAME_GAMEPAD_DPAD_DOWN	   13
#define GAME_GAMEPAD_DPAD_LEFT	   14
#define GAME_GAMEPAD_LEFT_TRIGGER  4
#define GAME_GAMEPAD_RIGHT_TRIGGER 5
#define GAME_GAMEPAD_LAST		   GAME_GAMEPAD_RIGHT_TRIGGER

#define GAME_KEY_UNKNOWN -1
#define GAME_KEY_A		 0x00
#define GAME_KEY_B		 0x0B
#define GAME_KEY_C		 0x08
#define GAME_KEY_D		 0x02
#define GAME_KEY_E		 0x0E
#define GAME_KEY_F		 0x03
#define GAME_KEY_G		 0x05
#define GAME_KEY_H		 0x04
#define GAME_KEY_I		 0x22
#define GAME_KEY_J		 0x26
#define GAME_KEY_K		 0x28
#define GAME_KEY_L		 0x25
#define GAME_KEY_M		 0x2E
#define GAME_KEY_N		 0x2D
#define GAME_KEY_O		 0x1F
#define GAME_KEY_P		 0x23
#define GAME_KEY_Q		 0x0C
#define GAME_KEY_R		 0x0F
#define GAME_KEY_S		 0x01
#define GAME_KEY_T		 0x11
#define GAME_KEY_U		 0x20
#define GAME_KEY_V		 0x09
#define GAME_KEY_W		 0x0D
#define GAME_KEY_X		 0x07
#define GAME_KEY_Y		 0x10
#define GAME_KEY_Z		 0x06

#define GAME_KEY_0 0x1D
#define GAME_KEY_1 0x12
#define GAME_KEY_2 0x13
#define GAME_KEY_3 0x14
#define GAME_KEY_4 0x15
#define GAME_KEY_5 0x17
#define GAME_KEY_6 0x16
#define GAME_KEY_7 0x1A
#define GAME_KEY_8 0x1C
#define GAME_KEY_9 0x19

#define GAME_KEY_RETURN	   0x24
#define GAME_KEY_ESCAPE	   0x35
#define GAME_KEY_BACKSPACE 0x33
#define GAME_KEY_TAB	   0x30
#define GAME_KEY_SPACE	   0x31

#define GAME_KEY_MINUS		   0x1B	 // '-'
#define GAME_KEY_TILDE		   0x32	 // '`'
#define GAME_KEY_ANGLE_BRACKET 0x102 // '`'
#define GAME_KEY_COMMA		   0x2B	 // ','
#define GAME_KEY_PERIOD		   0x2F	 // '.'
#define GAME_KEY_SLASH		   0x2C	 // '/'
#define GAME_KEY_CAPSLOCK	   0x39

#define GAME_KEY_F1	 0x7A
#define GAME_KEY_F2	 0x78
#define GAME_KEY_F3	 0x63
#define GAME_KEY_F4	 0x76
#define GAME_KEY_F5	 0x60
#define GAME_KEY_F6	 0x61
#define GAME_KEY_F7	 0x62
#define GAME_KEY_F8	 0x64
#define GAME_KEY_F9	 0x65
#define GAME_KEY_F10 0x6D
#define GAME_KEY_F11 0x67
#define GAME_KEY_F12 0x6F
#define GAME_KEY_F13 0x69
#define GAME_KEY_F14 0x6B
#define GAME_KEY_F15 0x71

#define GAME_KEY_PRINTSCREEN GAME_KEY_UNKNOWN
#define GAME_KEY_SCROLLLOCK	 GAME_KEY_UNKNOWN
#define GAME_KEY_PAUSE		 GAME_KEY_UNKNOWN

#define GAME_KEY_INSERT		  0x72 // "Help" key on some older Mac keyboards
#define GAME_KEY_HOME		  0x73
#define GAME_KEY_PAGEUP		  0x74
#define GAME_KEY_DELETE		  0x75
#define GAME_KEY_END		  0x77
#define GAME_KEY_PAGEDOWN	  0x79
#define GAME_KEY_RIGHT		  0x7C
#define GAME_KEY_LEFT		  0x7B
#define GAME_KEY_DOWN		  0x7D
#define GAME_KEY_UP			  0x7E
#define GAME_KEY_NUMLOCKCLEAR GAME_KEY_UNKNOWN

#define GAME_KEY_KP_DECIMAL	 0x41
#define GAME_KEY_KP_DIVIDE	 0x4B
#define GAME_KEY_KP_MULTIPLY 0x43
#define GAME_KEY_KP_MINUS	 0x4E
#define GAME_KEY_KP_PLUS	 0x45
#define GAME_KEY_KP_ENTER	 0x4C
#define GAME_KEY_KP_1		 0x53
#define GAME_KEY_KP_2		 0x54
#define GAME_KEY_KP_3		 0x55
#define GAME_KEY_KP_4		 0x56
#define GAME_KEY_KP_5		 0x57
#define GAME_KEY_KP_6		 0x58
#define GAME_KEY_KP_7		 0x59
#define GAME_KEY_KP_8		 0x5B
#define GAME_KEY_KP_9		 0x5C
#define GAME_KEY_KP_0		 0x52

#define GAME_KEY_LCTRL	0x3B
#define GAME_KEY_LSHIFT 0x38
#define GAME_KEY_LALT	0x3A // Option key
#define GAME_KEY_LGUI	0x37 // Command key
#define GAME_KEY_RCTRL	0x3E
#define GAME_KEY_RSHIFT 0x3C
#define GAME_KEY_RALT	0x3D // Option key
#define GAME_KEY_RGUI	0x36 // Command key

#endif

	enum class input_code
	{
		KeyUnknown		  = GAME_KEY_UNKNOWN,
		KeyA			  = GAME_KEY_A,
		KeyB			  = GAME_KEY_B,
		KeyC			  = GAME_KEY_C,
		KeyD			  = GAME_KEY_D,
		KeyE			  = GAME_KEY_E,
		KeyF			  = GAME_KEY_F,
		KeyG			  = GAME_KEY_G,
		KeyH			  = GAME_KEY_H,
		KeyI			  = GAME_KEY_I,
		KeyJ			  = GAME_KEY_J,
		KeyK			  = GAME_KEY_K,
		KeyL			  = GAME_KEY_L,
		KeyM			  = GAME_KEY_M,
		KeyN			  = GAME_KEY_N,
		KeyO			  = GAME_KEY_O,
		KeyP			  = GAME_KEY_P,
		KeyQ			  = GAME_KEY_Q,
		KeyR			  = GAME_KEY_R,
		KeyS			  = GAME_KEY_S,
		KeyT			  = GAME_KEY_T,
		KeyU			  = GAME_KEY_U,
		KeyV			  = GAME_KEY_V,
		KeyW			  = GAME_KEY_W,
		KeyX			  = GAME_KEY_X,
		KeyY			  = GAME_KEY_Y,
		KeyZ			  = GAME_KEY_Z,
		KeyAlpha0		  = GAME_KEY_0,
		KeyAlpha1		  = GAME_KEY_1,
		KeyAlpha2		  = GAME_KEY_2,
		KeyAlpha3		  = GAME_KEY_3,
		KeyAlpha4		  = GAME_KEY_4,
		KeyAlpha5		  = GAME_KEY_5,
		KeyAlpha6		  = GAME_KEY_6,
		KeyAlpha7		  = GAME_KEY_7,
		KeyAlpha8		  = GAME_KEY_8,
		KeyAlpha9		  = GAME_KEY_9,
		KeyReturn		  = GAME_KEY_RETURN,
		KeyEscape		  = GAME_KEY_ESCAPE,
		KeyBackspace	  = GAME_KEY_BACKSPACE,
		KeyTab			  = GAME_KEY_TAB,
		KeySpace		  = GAME_KEY_SPACE,
		KeyMinus		  = GAME_KEY_MINUS,
		KeyTilde		  = GAME_KEY_TILDE,
		KeyAngleBracket	  = GAME_KEY_ANGLE_BRACKET,
		KeyComma		  = GAME_KEY_COMMA,
		KeyPeriod		  = GAME_KEY_PERIOD,
		KeySlash		  = GAME_KEY_SLASH,
		KeyCapslock		  = GAME_KEY_CAPSLOCK,
		KeyF1			  = GAME_KEY_F1,
		KeyF2			  = GAME_KEY_F2,
		KeyF3			  = GAME_KEY_F3,
		KeyF4			  = GAME_KEY_F4,
		KeyF5			  = GAME_KEY_F5,
		KeyF6			  = GAME_KEY_F6,
		KeyF7			  = GAME_KEY_F7,
		KeyF8			  = GAME_KEY_F8,
		KeyF9			  = GAME_KEY_F9,
		KeyF10			  = GAME_KEY_F10,
		KeyF11			  = GAME_KEY_F11,
		KeyF12			  = GAME_KEY_F12,
		KeyPrintscreen	  = GAME_KEY_PRINTSCREEN,
		KeyScrolllock	  = GAME_KEY_SCROLLLOCK,
		KeyPause		  = GAME_KEY_PAUSE,
		KeyInsert		  = GAME_KEY_INSERT,
		KeyHome			  = GAME_KEY_HOME,
		KeyPageup		  = GAME_KEY_PAGEUP,
		KeyDelete		  = GAME_KEY_DELETE,
		KeyEnd			  = GAME_KEY_END,
		KeyPagedown		  = GAME_KEY_PAGEDOWN,
		KeyRight		  = GAME_KEY_RIGHT,
		KeyLeft			  = GAME_KEY_LEFT,
		KeyDown			  = GAME_KEY_DOWN,
		KeyUp			  = GAME_KEY_UP,
		KeyNumlockClear	  = GAME_KEY_NUMLOCKCLEAR,
		KeyKeypadDecimal  = GAME_KEY_KP_DECIMAL,
		KeyKeypadDivide	  = GAME_KEY_KP_DIVIDE,
		KeyKeypadMultiply = GAME_KEY_KP_MULTIPLY,
		KeyKeypadMinus	  = GAME_KEY_KP_MINUS,
		KeyKeypadPlus	  = GAME_KEY_KP_PLUS,
		KeyKeypadEnter	  = GAME_KEY_KP_ENTER,
		KeyKeypad1		  = GAME_KEY_KP_1,
		KeyKeypad2		  = GAME_KEY_KP_2,
		KeyKeypad3		  = GAME_KEY_KP_3,
		KeyKeypad4		  = GAME_KEY_KP_4,
		KeyKeypad5		  = GAME_KEY_KP_5,
		KeyKeypad6		  = GAME_KEY_KP_6,
		KeyKeypad7		  = GAME_KEY_KP_7,
		KeyKeypad8		  = GAME_KEY_KP_8,
		KeyKeypad9		  = GAME_KEY_KP_9,
		KeyKeypad0		  = GAME_KEY_KP_0,
		KeyF13			  = GAME_KEY_F13,
		KeyF14			  = GAME_KEY_F14,
		KeyF15			  = GAME_KEY_F15,
		KeyLCTRL		  = GAME_KEY_LCTRL,
		KeyLSHIFT		  = GAME_KEY_LSHIFT,
		KeyLALT			  = GAME_KEY_LALT,
		KeyLGUI			  = GAME_KEY_LGUI,
		KeyRCTRL		  = GAME_KEY_RCTRL,
		KeyRSHIFT		  = GAME_KEY_RSHIFT,
		KeyRALT			  = GAME_KEY_RALT,
		KeyRGUI			  = GAME_KEY_RGUI,

		MouseUnknown	= GAME_KEY_UNKNOWN,
		Mouse0			= GAME_MOUSE_0,
		Mouse1			= GAME_MOUSE_1,
		Mouse2			= GAME_MOUSE_2,
		Mouse3			= GAME_MOUSE_3,
		Mouse4			= GAME_MOUSE_4,
		Mouse5			= GAME_MOUSE_5,
		Mouse6			= GAME_MOUSE_6,
		Mouse7			= GAME_MOUSE_7,
		MouseLast		= GAME_MOUSE_LAST,
		MouseLeft		= GAME_MOUSE_LEFT,
		MouseRight		= GAME_MOUSE_RIGHT,
		MouseMiddle		= GAME_MOUSE_MIDDLE,
		JoystickUnknown = GAME_KEY_UNKNOWN,

		GamepadUnknown	 = GAME_KEY_UNKNOWN,
		GamepadA		 = GAME_GAMEPAD_A,
		GamepadB		 = GAME_GAMEPAD_B,
		GamepadX		 = GAME_GAMEPAD_X,
		GamepadY		 = GAME_GAMEPAD_Y,
		GamepadLBumper	 = GAME_GAMEPAD_LEFT_BUMPER,
		GamepadRBumper	 = GAME_GAMEPAD_RIGHT_BUMPER,
		GamepadBack		 = GAME_GAMEPAD_BACK,
		GamepadStart	 = GAME_GAMEPAD_START,
		GamepadGuide	 = GAME_GAMEPAD_GUIDE,
		GamepadLThumb	 = GAME_GAMEPAD_LEFT_THUMB,
		GamepadRThumb	 = GAME_GAMEPAD_RIGHT_THUMB,
		GamepadDpadUp	 = GAME_GAMEPAD_DPAD_UP,
		GamepadDpadRight = GAME_GAMEPAD_DPAD_RIGHT,
		GamepadDpadDown	 = GAME_GAMEPAD_DPAD_DOWN,
		GamepadDpadLeft	 = GAME_GAMEPAD_DPAD_LEFT,
		GamepadDpadLast	 = GAME_GAMEPAD_LAST,
		GamepadLTrigger	 = GAME_GAMEPAD_LEFT_TRIGGER,
		GamepadRTrigger	 = GAME_GAMEPAD_RIGHT_TRIGGER,
		GamepadLast		 = GAME_GAMEPAD_LAST,
	};

} // namespace SFG

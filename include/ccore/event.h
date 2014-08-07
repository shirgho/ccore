//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                //
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License as published by        //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

#pragma once

#include "core.h"
#include "types.h"

// A type of event returned used in ccWindow after the function ccPollEvent is called
typedef enum {
	//Event not handled in ccore, return false on pollEvent
	CC_EVENT_SKIP,
	//A key is pressed, store keycode
	CC_EVENT_KEY_DOWN,
	//A key is released, store keycode
	CC_EVENT_KEY_UP,
	//Mouse has moved, store coordinates
	CC_EVENT_MOUSE_MOVE,
	//Mouse button down, store buttoncode
	CC_EVENT_MOUSE_DOWN,
	//Mouse button up, store buttoncode
	CC_EVENT_MOUSE_UP,
	//Mouse wheel moved up
	CC_EVENT_MOUSE_SCROLL,
	//The keyboard is focussed on the window
	CC_EVENT_FOCUS_GAINED,
	//The keyboard lost focus on the window
	CC_EVENT_FOCUS_LOST,
	//Window close button was pressed
	CC_EVENT_WINDOW_QUIT,
	//Window has resized, new dimensions are stored in the window struct before the event fires
	CC_EVENT_WINDOW_RESIZE,
} ccEventType;

// The type of mouse button pressed, used inside the ccEvent union
typedef enum {
	CC_MOUSE_BUTTON_NONE,
	CC_MOUSE_BUTTON_LEFT,
	CC_MOUSE_BUTTON_MIDDLE,
	CC_MOUSE_BUTTON_RIGHT
} ccMouseButtonType;

// The event structure used by ccWindow, set by the call ccPollEvent
typedef struct {
	ccEventType type;
	// The union contains different data types for different events
	union {
		ccMouseButtonType mouseButton;
		ccPoint mouseVector;
		unsigned int keyCode;
		float scrollDelta;
	};
} ccEvent;

#ifdef X11

#include <X11/keysym.h>

#define CC_KEY_0 XK_0
#define CC_KEY_1 XK_1
#define CC_KEY_2 XK_2
#define CC_KEY_3 XK_3
#define CC_KEY_4 XK_4
#define CC_KEY_5 XK_5
#define CC_KEY_6 XK_6
#define CC_KEY_7 XK_7
#define CC_KEY_8 XK_8
#define CC_KEY_9 XK_9

#define CC_KEY_A XK_a
#define CC_KEY_B XK_b
#define CC_KEY_C XK_c
#define CC_KEY_D XK_d
#define CC_KEY_E XK_e
#define CC_KEY_F XK_f
#define CC_KEY_G XK_g
#define CC_KEY_H XK_h
#define CC_KEY_I XK_i
#define CC_KEY_J XK_j
#define CC_KEY_K XK_k
#define CC_KEY_L XK_l
#define CC_KEY_M XK_m
#define CC_KEY_N XK_n
#define CC_KEY_O XK_o
#define CC_KEY_P XK_p
#define CC_KEY_Q XK_q
#define CC_KEY_R XK_r
#define CC_KEY_S XK_s
#define CC_KEY_T XK_t
#define CC_KEY_U XK_u
#define CC_KEY_V XK_v
#define CC_KEY_W XK_w
#define CC_KEY_X XK_x
#define CC_KEY_Y XK_y
#define CC_KEY_Z XK_z

#define CC_KEY_F1 XK_F1
#define CC_KEY_F2 XK_F2
#define CC_KEY_F3 XK_F3
#define CC_KEY_F4 XK_F4
#define CC_KEY_F5 XK_F5
#define CC_KEY_F6 XK_F6
#define CC_KEY_F7 XK_F7
#define CC_KEY_F8 XK_F8
#define CC_KEY_F9 XK_F9
#define CC_KEY_F10 XK_F10
#define CC_KEY_F11 XK_F11
#define CC_KEY_F12 XK_F12

#define CC_KEY_NUM0 XK_KP_0
#define CC_KEY_NUM1 XK_KP_1
#define CC_KEY_NUM2 XK_KP_2
#define CC_KEY_NUM3 XK_KP_3
#define CC_KEY_NUM4 XK_KP_4
#define CC_KEY_NUM5 XK_KP_5
#define CC_KEY_NUM6 XK_KP_6
#define CC_KEY_NUM7 XK_KP_7
#define CC_KEY_NUM8 XK_KP_8
#define CC_KEY_NUM9 XK_KP_9

#define CC_KEY_BACKSPACE XK_BackSpace
#define CC_KEY_TAB XK_Tab
#define CC_KEY_RETURN XK_Return
#define CC_KEY_ESCAPE XK_Escape
#define CC_KEY_SPACE XK_space
#define CC_KEY_CAPSLOCK XK_Caps_Lock
#define CC_KEY_INSERT XK_Insert
#define CC_KEY_DELETE XK_Delete
#define CC_KEY_HOME XK_Home
#define CC_KEY_END XK_End
#define CC_KEY_PAGEUP XK_Page_Up
#define CC_KEY_PAGEDOWN XK_Page_Down
#define CC_KEY_PRINTSCREEN XK_Print
#define CC_KEY_SCROLLLOCK XK_Scroll_Lock
#define CC_KEY_NUMLOCK XK_Num_Lock
#define CC_KEY_PAUSEBREAK XK_Pause

#define CC_KEY_LSHIFT XK_Shift_L
#define CC_KEY_RSHIFT XK_Shift_R
#define CC_KEY_LCONTROL XK_Control_L
#define CC_KEY_RCONTROL XK_Control_R

#define CC_KEY_LEFT XK_Left
#define CC_KEY_UP XK_Up
#define CC_KEY_RIGHT XK_Right
#define CC_KEY_DOWN XK_Down

// Not implemented in Windows
#define CC_KEY_LALT XK_Alt_L
#define CC_KEY_RALT XK_Alt_R

#define CC_KEY_LMETA XK_Meta_L
#define CC_KEY_RMETA XK_Meta_R

#elif defined WIN32

#include <Windows.h>

#define CC_KEY_0 '0'
#define CC_KEY_1 '1'
#define CC_KEY_2 '2'
#define CC_KEY_3 '3'
#define CC_KEY_4 '4'
#define CC_KEY_5 '5'
#define CC_KEY_6 '6'
#define CC_KEY_7 '7'
#define CC_KEY_8 '8'
#define CC_KEY_9 '9'

#define CC_KEY_A 'A'
#define CC_KEY_B 'B'
#define CC_KEY_C 'C'
#define CC_KEY_D 'D'
#define CC_KEY_E 'E'
#define CC_KEY_F 'F'
#define CC_KEY_G 'G'
#define CC_KEY_H 'H'
#define CC_KEY_I 'I'
#define CC_KEY_J 'J'
#define CC_KEY_K 'K'
#define CC_KEY_L 'L'
#define CC_KEY_M 'M'
#define CC_KEY_N 'N'
#define CC_KEY_O 'O'
#define CC_KEY_P 'P'
#define CC_KEY_Q 'Q'
#define CC_KEY_R 'R'
#define CC_KEY_S 'S'
#define CC_KEY_T 'T'
#define CC_KEY_U 'U'
#define CC_KEY_V 'V'
#define CC_KEY_W 'W'
#define CC_KEY_X 'X'
#define CC_KEY_Y 'Y'
#define CC_KEY_Z 'Z'

#define CC_KEY_F1 VK_F1
#define CC_KEY_F2 VK_F2
#define CC_KEY_F3 VK_F3
#define CC_KEY_F4 VK_F4
#define CC_KEY_F5 VK_F5
#define CC_KEY_F6 VK_F6
#define CC_KEY_F7 VK_F7
#define CC_KEY_F8 VK_F8
#define CC_KEY_F9 VK_F9
#define CC_KEY_F10 VK_F10
#define CC_KEY_F11 VK_F11
#define CC_KEY_F12 VK_F12

#define CC_KEY_NUM0 VK_NUMPAD0
#define CC_KEY_NUM1 VK_NUMPAD1
#define CC_KEY_NUM2 VK_NUMPAD2
#define CC_KEY_NUM3 VK_NUMPAD3
#define CC_KEY_NUM4 VK_NUMPAD4
#define CC_KEY_NUM5 VK_NUMPAD5
#define CC_KEY_NUM6 VK_NUMPAD6
#define CC_KEY_NUM7 VK_NUMPAD7
#define CC_KEY_NUM8 VK_NUMPAD8
#define CC_KEY_NUM9 VK_NUMPAD9

#define CC_KEY_BACKSPACE VK_BACK
#define CC_KEY_TAB VK_TAB
#define CC_KEY_RETURN VK_RETURN
#define CC_KEY_ESCAPE VK_ESCAPE
#define CC_KEY_SPACE VK_SPACE
#define CC_KEY_CAPSLOCK VK_CAPITAL
#define CC_KEY_INSERT VK_INSERT
#define CC_KEY_DELETE VK_DELETE
#define CC_KEY_HOME VK_HOME
#define CC_KEY_END VK_END
#define CC_KEY_PAGEUP VK_PRIOR
#define CC_KEY_PAGEDOWN VK_NEXT
#define CC_KEY_PRINTSCREEN VK_SNAPSHOT
#define CC_KEY_SCROLLLOCK VK_SCROLL
#define CC_KEY_NUMLOCK VK_NUMLOCK
#define CC_KEY_PAUSEBREAK VK_PAUSE

#define CC_KEY_LSHIFT VK_LSHIFT
#define CC_KEY_RSHIFT VK_RSHIFT
#define CC_KEY_LCONTROL VK_LCONTROL
#define CC_KEY_RCONTROL VK_RCONTROL

#define CC_KEY_LEFT VK_LEFT
#define CC_KEY_UP VK_UP
#define CC_KEY_RIGHT VK_RIGHT
#define CC_KEY_DOWN VK_DOWN

#endif

char ccKeyToChar(int key);

//__________________________________________________________________________________\\
//                               ______                                             \\
//                              /  ___ /__  _ __ ___                                \\
//                             /  / __/ _ \| '__/ _ \                               \\
//                            |  | (_| (_) | | |  __/                               \\
//                             \  \___\___/|_|  \___| 1.0                           \\
//                              \______\                                            \\
//                                                                                  \\
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                \\
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       \\
//__________________________________________________________________________________\\
//                                                                                  \\
//      This program is free software: you can redistribute it and/or modify        \\
//      it under the terms of the GNU General Public License as published by        \\
//      the Free Software Foundation.                                               \\
//                                                                                  \\
//      This program is distributed without any warranty; see the GNU               \\
//      General Public License for more details.                                    \\
//                                                                                  \\
//      You should have received a copy of the GNU General Public License           \\
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        \\
//__________________________________________________________________________________\\

#pragma once

#include "core.h"
#include "types.h"

#ifdef X11
#include "../../src/ccore/x11/interface/event.h"
#elif defined WIN32
#include "../../src/ccore/win32/interface/event.h"
#endif

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

char ccKeyToChar(int key);

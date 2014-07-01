#pragma once

#include "../types.h"

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
	CC_MOUSE_BUTTON_NONE = 0,
	CC_MOUSE_BUTTON_LEFT = 1,
	CC_MOUSE_BUTTON_MIDDLE = 2,
	CC_MOUSE_BUTTON_RIGHT = 3,
	CC_MOUSE_BUTTON_SPECIAL_1 = 4,
	CC_MOUSE_BUTTON_SPECIAL_2 = 5
} ccMouseButtonType;

// A type of keyboard action, used inside the ccEvent union
// Mapped on ASCII where possible
typedef enum {
	CC_KEY_UNDEFINED = 0,

	CC_KEY_BACKSPACE = 8,
 	CC_KEY_TAB = '\t',
	CC_KEY_RETURN = 13,
	CC_KEY_ESCAPE = 27,
	CC_KEY_SPACE = ' ',

	CC_KEY_0 = '0',
	CC_KEY_1 = '1',
	CC_KEY_2 = '2',
	CC_KEY_3 = '3',
	CC_KEY_4 = '4',
	CC_KEY_5 = '5',
	CC_KEY_6 = '6',
	CC_KEY_7 = '7',
	CC_KEY_8 = '8',
	CC_KEY_9 = '9',
	CC_KEY_A = 'A',
	CC_KEY_B = 'B',
	CC_KEY_C = 'C',
	CC_KEY_D = 'D',
	CC_KEY_E = 'E',
	CC_KEY_F = 'F',
	CC_KEY_G = 'G',
	CC_KEY_H = 'H',
	CC_KEY_I = 'I',
	CC_KEY_J = 'J',
	CC_KEY_K = 'K',
	CC_KEY_L = 'L',
	CC_KEY_M = 'M',
	CC_KEY_N = 'N',
	CC_KEY_O = 'O',
	CC_KEY_P = 'P',
	CC_KEY_Q = 'Q',
	CC_KEY_R = 'R',
	CC_KEY_S = 'S',
	CC_KEY_T = 'T',
	CC_KEY_U = 'U',
	CC_KEY_V = 'V',
	CC_KEY_W = 'W',
	CC_KEY_X = 'X',
	CC_KEY_Y = 'Y',
	CC_KEY_Z = 'Z',

	CC_KEY_CAPSLOCK,
	CC_KEY_INSERT,
	CC_KEY_DELETE,
	CC_KEY_HOME,
	CC_KEY_END,
	CC_KEY_PAGEUP,
	CC_KEY_PAGEDOWN,

	CC_KEY_PRINTSCREEN,
	CC_KEY_SCROLLLOCK,
	CC_KEY_NUMLOCK,
	CC_KEY_PAUSEBREAK,

	CC_KEY_F1,
	CC_KEY_F2,
	CC_KEY_F3,
	CC_KEY_F4,
	CC_KEY_F5,
	CC_KEY_F6,
	CC_KEY_F7,
	CC_KEY_F8,
	CC_KEY_F9,
	CC_KEY_F10,
	CC_KEY_F11,
	CC_KEY_F12,

	CC_KEY_NUM0,
	CC_KEY_NUM1,
	CC_KEY_NUM2,
	CC_KEY_NUM3,
	CC_KEY_NUM4,
	CC_KEY_NUM5,
	CC_KEY_NUM6,
	CC_KEY_NUM7,
	CC_KEY_NUM8,
	CC_KEY_NUM9,

	CC_KEY_LCONTROL,
	CC_KEY_RCONTROL,
	CC_KEY_LSHIFT,
	CC_KEY_RSHIFT,
	CC_KEY_LMENU,
	CC_KEY_RMENU,

	CC_KEY_LEFT,
	CC_KEY_RIGHT,
	CC_KEY_UP,
	CC_KEY_DOWN
} ccKeyCode;

typedef struct {
	ccKeyCode keyCode; //CCORE supported key code described in the enum above
	unsigned int scanCode; //hardware dependent unique scancode, for custom key mapping
} ccKeyData;

// The event structure used by ccWindow, set by the call ccPollEvent
typedef struct {
	// The type of data inside the union depends on the type of event
	ccEventType type;
	// The union contains different data types for different events
	union {
		ccMouseButtonType mouseButton;
		ccPoint mousePosition;
		ccKeyData key;
		short scrollDelta;
	};
} ccEvent;

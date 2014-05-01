#pragma once

#include "../types.h"

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

typedef enum {
	CC_MOUSE_BUTTON_NONE = 0,
	CC_MOUSE_BUTTON_LEFT = 1,
	CC_MOUSE_BUTTON_MIDDLE = 2,
	CC_MOUSE_BUTTON_RIGHT = 3,
	CC_MOUSE_BUTTON_SPECIAL_1 = 4,
	CC_MOUSE_BUTTON_SPECIAL_2 = 5
} ccMouseButtonType;

// Partially mapped on ascii where possible
typedef enum {
	CC_KEY_UNDEFINED = 0,

	CC_KEY_BACKSPACE = 8,
 	CC_KEY_TAB = '\t',
	CC_KEY_RETURN = 13,
	CC_KEY_ESCAPE = 27,
	CC_KEY_SPACE = ' ',

	CC_KEY_LSHIFT,
	CC_KEY_RSHIFT,
	CC_KEY_LCONTROL,
	CC_KEY_RCONTROL,

	CC_KEY_LEFT,
	CC_KEY_RIGHT,
	CC_KEY_UP,
	CC_KEY_DOWN,

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
	CC_KEY_Z = 'Z'
} ccKeyCode;

typedef struct {
	ccEventType type;
	union {
		ccMouseButtonType mouseButton;
		ccPoint mousePosition;
		ccKeyCode key;
		int scrollDelta;
	};
} ccEvent;

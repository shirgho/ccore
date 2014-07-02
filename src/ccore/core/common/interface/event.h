#pragma once

#include "../types.h"

#define CC_KEY_UNDEFINED 0

#define CC_KEY_(NUMBER) ('NUMBER')

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

// The event structure used by ccWindow, set by the call ccPollEvent
typedef struct {
	// The type of data inside the union depends on the type of event
	ccEventType type;
	// The union contains different data types for different events
	union {
		ccMouseButtonType mouseButton;
		ccPoint mousePosition;
		char keyCode;
		short scrollDelta;
	};
} ccEvent;

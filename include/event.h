#pragma once

#include "core.h"
#include "types.h"

#ifdef X11
#include "../src/x11/interface/event.h"
#elif defined WIN32
#include "../src/win32/interface/event.h"
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
	// The type of data inside the union depends on the type of event
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

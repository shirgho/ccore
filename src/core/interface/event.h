#pragma once

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
	//Mouse wheel moved down
	CC_EVENT_MOUSE_SCROLL_DOWN,
	//Mouse wheel moved up
	CC_EVENT_MOUSE_SCROLL_UP,
	//Mouse scrolled over the windows bounds
	CC_EVENT_MOUSE_FOCUS_GAINED,
	//Mouse outside the windows bounds
	CC_EVENT_MOUSE_FOCUS_LOST,
	//The keyboard is focussed on the window
	CC_EVENT_KEYBOARD_FOCUS_GAINED,
	//The keyboard lost focus on the window
	CC_EVENT_KEYBOARD_FOCUS_LOST,
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

//TODO: is a struct necessary since coordinates are now included in the window?
typedef struct {
	ccMouseButtonType button;
} ccMouseState;

typedef struct {
	ccEventType type;
	union {
		ccMouseState mouseState;
		char key;
	};
} ccEvent;

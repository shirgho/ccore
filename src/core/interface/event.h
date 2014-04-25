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
	CC_MOUSE_BUTTON_LEFT,
	CC_MOUSE_BUTTON_MIDDLE,
	CC_MOUSE_BUTTON_RIGHT,
	CC_MOUSE_BUTTON_SPECIAL_1,
	CC_MOUSE_BUTTON_SPECIAL_2
} ccMouseButtonType;

typedef struct {
	ccEventType type;
	union {
		ccMouseButtonType mouseButton;
		char key;
	};
} ccEvent;

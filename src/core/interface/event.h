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
	CC_MOUSE_BUTTON_LEFT,
	CC_MOUSE_BUTTON_MIDDLE,
	CC_MOUSE_BUTTON_RIGHT,
	CC_MOUSE_BUTTON_SPECIAL_1,
	CC_MOUSE_BUTTON_SPECIAL_2
} ccMouseButtonType;

typedef enum {
	CC_KEY_BACKSPACE,
	CC_KEY_TAB,
	CC_KEY_RETURN,
	CC_KEY_ESCAPE,
	CC_KEY_SPACE,
	CC_KEY_LEFT,
	CC_KEY_RIGHT,
	CC_KEY_UP,
	CC_KEY_DOWN,
	CC_KEY_0,
	CC_KEY_1,
	CC_KEY_2,
	CC_KEY_3,
	CC_KEY_4,
	CC_KEY_5,
	CC_KEY_6,
	CC_KEY_7,
	CC_KEY_8,
	CC_KEY_9,
	CC_KEY_A,
	CC_KEY_B,
	CC_KEY_C,
	CC_KEY_D,
	CC_KEY_E,
	CC_KEY_F,
	CC_KEY_G,
	CC_KEY_H,
	CC_KEY_I,
	CC_KEY_J,
	CC_KEY_K,
	CC_KEY_L,
	CC_KEY_M,
	CC_KEY_N,
	CC_KEY_O,
	CC_KEY_P,
	CC_KEY_Q,
	CC_KEY_R,
	CC_KEY_S,
	CC_KEY_T,
	CC_KEY_U,
	CC_KEY_V,
	CC_KEY_W,
	CC_KEY_X,
	CC_KEY_Y,
	CC_KEY_Z
} ccKeyCode;

typedef struct {
	ccEventType type;
	union {
		ccMouseButtonType mouseButton;
		ccKeyCode key;
		short scrollDelta;
	};
} ccEvent;
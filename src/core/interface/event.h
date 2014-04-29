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
	CC_MOUSE_BUTTON_NONE = 0,
	CC_MOUSE_BUTTON_LEFT = 1,
	CC_MOUSE_BUTTON_MIDDLE = 2,
	CC_MOUSE_BUTTON_RIGHT = 3,
	CC_MOUSE_BUTTON_SPECIAL_1 = 4,
	CC_MOUSE_BUTTON_SPECIAL_2 = 5
} ccMouseButtonType;

//partially mapped on ascii where possible
typedef enum {
	CC_KEY_BACKSPACE = 8,
 	CC_KEY_TAB,
	CC_KEY_RETURN = 13,
	CC_KEY_ESCAPE = 27,
	CC_KEY_SPACE = 32,
	CC_KEY_LEFT,
	CC_KEY_RIGHT,
	CC_KEY_UP,
	CC_KEY_DOWN,
	CC_KEY_0 = 48,
	CC_KEY_1 = 49,
	CC_KEY_2 = 50,
	CC_KEY_3 = 51,
	CC_KEY_4 = 52,
	CC_KEY_5 = 53,
	CC_KEY_6 = 54,
	CC_KEY_7 = 55,
	CC_KEY_8 = 56,
	CC_KEY_9 = 57,
	CC_KEY_A = 65,
	CC_KEY_B = 66,
	CC_KEY_C = 67,
	CC_KEY_D = 68,
	CC_KEY_E = 69,
	CC_KEY_F = 70,
	CC_KEY_G = 71,
	CC_KEY_H = 72,
	CC_KEY_I = 73,
	CC_KEY_J = 74,
	CC_KEY_K = 75,
	CC_KEY_L = 76,
	CC_KEY_M = 77,
	CC_KEY_N = 78,
	CC_KEY_O = 79,
	CC_KEY_P = 80,
	CC_KEY_Q = 81,
	CC_KEY_R = 82,
	CC_KEY_S = 83,
	CC_KEY_T = 84,
	CC_KEY_U = 85,
	CC_KEY_V = 86,
	CC_KEY_W = 87,
	CC_KEY_X = 88,
	CC_KEY_Y = 89,
	CC_KEY_Z = 90
} ccKeyCode;

typedef struct {
	ccEventType type;
	union {
		ccMouseButtonType mouseButton;
		ccPoint mousePosition;
		ccKeyCode key;
		short scrollDelta;
	};
} ccEvent;

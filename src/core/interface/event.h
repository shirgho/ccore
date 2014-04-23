#pragma once

typedef enum {
	//Event not handled in ccore, return false on pollEvent
	ccEventSkip,
	//A key is pressed, store keycode
	ccEventKeyDown,
	//A key is released, store keycode
	ccEventKeyUp,
	//Mouse has moved, store coordinates
	ccEventMouseMove,
	//Mouse button down, store buttoncode
	ccEventMouseDown,
	//Mouse button up, store buttoncode
	ccEventMouseUp,
	//Mouse wheel moved down
	ccEventMouseScrollDown,
	//Mouse wheel moved up
	ccEventMouseScrollUp,
	//Mouse scrolled over the windows bounds
	ccEventMouseFocusGained,
	//Mouse outside the windows bounds
	ccEventMouseFocusLost,
	//The keyboard is focussed on the window
	ccEventKeyboardFocusGained,
	//The keyboard lost focus on the window
	ccEventKeyboardFocusLost,
	//Window close button was pressed
	ccEventWindowQuit,
	//Window has resized, new dimensions are stored in the window struct before the event fires
	ccEventWindowResize,
} ccEventType;

typedef enum {
	ccMouseButtonNone = 0,
	ccMouseButtonLeft = 1,
	ccMouseButtonMiddle = 2,
	ccMouseButtonRight = 3,
	ccMouseButtonSpecial1 = 4,
	ccMouseButtonSpecial2 = 5
} ccMouseButtonType;

typedef struct {
	ccPoint location;
	ccMouseButtonType button;
} ccMouseState;

typedef struct {
	ccEventType type;
	union {
		ccMouseState mouseState;
	};
} ccEvent;

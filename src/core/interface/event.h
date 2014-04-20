#pragma once

typedef enum {
	//event not handled in ccore, return false on pollEvent
	ccEventSkip,
	//a key is pressed, store keycode
	ccEventKeyDown,
	//a key is released, store keycode
	ccEventKeyUp,
	//mouse has moved, store coordinates
	ccEventMouseMove,
	//mouse button down, store buttoncode
	ccEventMouseDown,
	//mouse button up, store buttoncode
	ccEventMouseUp,
	//mouse wheel moved down
	ccEventMouseScrollDown,
	//mouse wheel moved up
	ccEventMouseScrollUp,
	//window close button was pressed
	ccEventWindowQuit,
	//window has resized, new dimensions are stored in the window struct before the event fires
	ccEventWindowResize,
} ccEventType;

typedef struct {
	ccEventType type;
	union {
		ccPoint mouseLocation;
		
	};
} ccEvent;

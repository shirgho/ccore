#pragma once

typedef enum {
	ccEventSkip,
	ccEventKeyDown,
	ccEventKeyUp,
	ccEventMouseMove,
	ccEventMouseDown,
	ccEventMouseUp,
	ccEventMouseScrollDown,
	ccEventMouseScrollUp,
	ccEventWindowQuit,
	ccEventWindowResize,
} ccEventType;

typedef struct {
	ccEventType type;
	unsigned int data;
} ccEvent;
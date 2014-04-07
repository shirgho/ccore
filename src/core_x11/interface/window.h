#pragma once

#include <stdlib.h>
#include <X11/Xlib.h>

typedef struct {
	unsigned short width;
	unsigned short height;
	Display *display;
	Window window;
} ccWindow;

ccWindow *ccNewWindow(unsigned short width, unsigned short height);
void ccFreeWindow(ccWindow *window);

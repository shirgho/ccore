#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <ccore/opengl.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <ccore/window.h>
#include <ccore/gamepad.h>

#include <ccore/types.h>
#include <ccore/event.h>
#include <ccore/error.h>
#include <ccore/assert.h>
#include <ccore/print.h>

#ifdef LINUX
#include "../../linux/interface/lin_gamepad.h"
#endif

#include "x11_display.h"

typedef struct {
	Display *XDisplay;
	Window XWindow;
	int XScreen;
	int windowFlags;
	bool resizable;
	GLXContext XContext;
} ccWindow_x11;

#define WINDOW_DATA ((ccWindow_x11*)_window->data)

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#include "../../core/types.h"
#include "../../core/interface/event.h"
#include "../../core/utils/error.h"

typedef struct {
	unsigned short width;
	unsigned short height;
	ccEvent event;

	bool(*pollEvent)();

	Display *display;
	Window window;
} ccWindow;

#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>

#include "../Types.h"
#include "Event.h"
#include "../utils/Error.h"

typedef struct _ccWindow ccWindow;

typedef struct _ccWindow{
	unsigned short width;
	unsigned short height;
	ccEvent event;

	bool(*pollEvent)();

	MSG msg;
	HWND winHandle;
} ccWindow;

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title);
void ccFreeWindow(ccWindow *w);
#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>

#include "../types.h"
#include "event.h"
#include "../utils/error.h"

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
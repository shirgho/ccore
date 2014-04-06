#pragma once

#include <stdlib.h>
#include <Windows.h>

#include "../Types.h"

typedef struct {
	unsigned short width;
	unsigned short height;
} ccWindow;

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title);
void ccFreeWindow(ccWindow *w);
#pragma once

#include <stdlib.h>
#include <Windows.h>

typedef struct {
	unsigned short width;
	unsigned short height;
} ccWindow;

ccWindow *newWindow(unsigned short width, unsigned short height);
void freeWindow(ccWindow *w);
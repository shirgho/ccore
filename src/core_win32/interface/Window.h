#pragma once

#include <Windows.h>

typedef struct {
	unsigned short width;
	unsigned short height;
} Window;

Window *newWindow(unsigned short width, unsigned short height);
void freeWindow(Window *w);
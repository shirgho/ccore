#include <stdlib.h>

#include "Window.h"

//Private

LRESULT CALLBACK WndProc(HWND Hw, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg) {
	default: return DefWindowProc(Hw, Msg, wParam, lParam);
	}
}

//Public

Window *newWindow(unsigned short width, unsigned short height)
{
	Window *w = malloc(sizeof(Window));



	return w;
}

void freeWindow(Window *w)
{
	free(w);
}
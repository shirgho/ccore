#include "Window.h"

//Private

LRESULT CALLBACK WndProc(HWND Hw, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg) {
	default: return DefWindowProc(Hw, Msg, wParam, lParam);
	}
}

//Public

ccWindow *newWindow(unsigned short width, unsigned short height)
{
	ccWindow *w = malloc(sizeof(ccWindow));



	return w;
}

void freeWindow(ccWindow *w)
{
	free(w);
}
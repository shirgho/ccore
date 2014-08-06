#include <ccore/window.h>

ccEvent ccGetEvent()
{
	ccAssert(_window != NULL);

	return _window->event;
}

ccRect ccGetWindowRect()
{
	ccAssert(_window != NULL);

	return _window->rect;
}

ccPoint ccGetWindowMouse()
{
	ccAssert(_window != NULL);

	return _window->mouse;
}

ccDisplay *ccGetWindowDisplay()
{
	ccAssert(_window != NULL);

	return _window->display;
}

bool ccWindowExists()
{
	return _window != NULL;
}

void ccUpdateWindowDisplay()
{
	int i;
	int area, largestArea;
	ccRect displayRect;

	ccAssert(_window != NULL);

	largestArea = 0;
	for(i = 0; i < ccGetDisplayAmount(); i++) {
		displayRect = ccGetDisplayRect(ccGetDisplay(i));
		area = ccRectIntersectionArea(&displayRect, &_window->rect);
		if(area > largestArea) {
			largestArea = area;
			_window->display = ccGetDisplay(i);
		}
	}
}
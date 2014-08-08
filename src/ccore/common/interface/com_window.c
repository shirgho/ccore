#include <ccore/window.h>

ccEvent ccWindowGetEvent()
{
	ccAssert(_window != NULL);

	return _window->event;
}

ccRect ccWindowGetRect()
{
	ccAssert(_window != NULL);

	return _window->rect;
}

ccPoint ccWindowGetMouse()
{
	ccAssert(_window != NULL);

	return _window->mouse;
}

ccDisplay *ccWindowGetDisplay()
{
	ccAssert(_window != NULL);

	return _window->display;
}

bool ccWindowExists()
{
	return _window != NULL;
}

void ccWindowUpdateDisplay()
{
	int i;
	int area, largestArea;
	ccRect displayRect;

	ccAssert(_window != NULL);

	largestArea = 0;
	for(i = 0; i < ccDisplayGetAmount(); i++) {
		displayRect = ccDisplayGetRect(ccDisplayGet(i));
		area = ccRectIntersectionArea(&displayRect, &_window->rect);
		if(area > largestArea) {
			largestArea = area;
			_window->display = ccDisplayGet(i);
		}
	}
}
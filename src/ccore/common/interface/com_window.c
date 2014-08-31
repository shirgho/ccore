#include <ccore/window.h>

ccEvent ccWindowGetEvent(void)
{
	ccAssert(_ccWindow != NULL);

	return _ccWindow->event;
}

ccRect ccWindowGetRect(void)
{
	ccAssert(_ccWindow != NULL);

	return _ccWindow->rect;
}

ccPoint ccWindowGetMouse(void)
{
	ccAssert(_ccWindow != NULL);

	return _ccWindow->mouse;
}

ccDisplay *ccWindowGetDisplay(void)
{
	ccAssert(_ccWindow != NULL);

	return _ccWindow->display;
}

bool ccWindowExists(void)
{
	return _ccWindow != NULL;
}

void ccWindowUpdateDisplay(void)
{
	int i;
	int area, largestArea;
	ccRect displayRect;

	ccAssert(_ccWindow != NULL);

	largestArea = 0;
	for(i = 0; i < ccDisplayGetAmount(); i++) {
		displayRect = ccDisplayGetRect(ccDisplayGet(i));
		area = ccRectIntersectionArea(&displayRect, &_ccWindow->rect);
		if(area > largestArea) {
			largestArea = area;
			_ccWindow->display = ccDisplayGet(i);
		}
	}
}
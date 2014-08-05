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
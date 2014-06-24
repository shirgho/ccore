#include "../../common/interface/window.h"

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

bool ccWindowExists()
{
	return _window != NULL;
}

void ccNewWindow(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

	ccAssert(_window == NULL);

	_window = malloc(sizeof(ccWindow));
	_window->XDisplay = XOpenDisplay(NULL);
	//TODO: change ccAssert to error
	ccAssert(_window->XDisplay != NULL);
	root = DefaultRootWindow(_window->XDisplay);
	_window->XScreen = DefaultScreen(_window->XDisplay);
	_window->XWindow = XCreateSimpleWindow(_window->XDisplay, root, rect.x, rect.y, rect.width, rect.height, 1, BlackPixel(_window->XDisplay, _window->XScreen), WhitePixel(_window->XDisplay, _window->XScreen));
	// Choose types of events
	XSelectInput(_window->XDisplay, _window->XWindow, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask);
	XMapWindow(_window->XDisplay, _window->XWindow);
	XStoreName(_window->XDisplay, _window->XWindow, title);

	_window->mouse.x = _window->mouse.y = _window->rect.width = _window->rect.height = -1;

	delete = XInternAtom(_window->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(_window->XDisplay, _window->XWindow, &delete, 1);
}

void ccFreeWindow()
{
	ccAssert(_window != NULL);
	//TODO: don't delete a context before checking whether it exists!
	/*
	glXMakeCurrent(_window->XDisplay, None, NULL);
	glXDestroyContext(_window->XDisplay, _window->XContext);
	*/

	XCloseDisplay(_window->XDisplay);
	free(_window);
}

bool ccPollEvent()
{
	XEvent event;
	XWindowAttributes _windowAttributes;

	ccAssert(_window != NULL);

	_window->event.type = CC_EVENT_SKIP;
	if(XPending(_window->XDisplay) == 0){
		return false;
	}

	XNextEvent(_window->XDisplay, &event);
	switch(event.type){
		case ButtonPress:
			// 1 = left, 2 = middle, 3 = right, 4 = scroll up, 5 = scroll down
			if(event.xbutton.button <= 3){
				_window->event.type = CC_EVENT_MOUSE_DOWN;
				_window->event.mouseButton = event.xbutton.button;
			}else if(event.xbutton.button == 4){
				_window->event.type = CC_EVENT_MOUSE_SCROLL;
				_window->event.scrollDelta = 1;
			}else if(event.xbutton.button == 5){
				_window->event.type = CC_EVENT_MOUSE_SCROLL;
				_window->event.scrollDelta = -1;
			}
			break;
		case ButtonRelease:
			_window->event.type = CC_EVENT_MOUSE_UP;
			_window->event.mouseButton = event.xbutton.button;
			break;
		case MotionNotify:
			if(_window->mouse.x != event.xmotion.x || _window->mouse.y != event.xmotion.y){
				_window->event.type = CC_EVENT_MOUSE_MOVE;
				_window->mouse.x = event.xmotion.x;
				_window->mouse.y = event.xmotion.y;
			}
			break;
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;
		case KeyPress:
			_window->event.type = CC_EVENT_KEY_DOWN;
			_window->event.key = ccXLookupKey(XLookupKeysym(&event.xkey, 0));
			break;
		case KeyRelease:
			_window->event.type = CC_EVENT_KEY_UP;
			_window->event.key = ccXLookupKey(XLookupKeysym(&event.xkey, 0));
			break;
		case ConfigureNotify:
			if(_window->rect.width != event.xconfigure.width || _window->rect.height != event.xconfigure.height){
				_window->event.type = CC_EVENT_WINDOW_RESIZE;
				_window->rect.width = event.xconfigure.width;
				_window->rect.height = event.xconfigure.height;
				_window->aspect = _window->rect.height / _window->rect.width;
				XGetWindowAttributes(_window->XDisplay, _window->XWindow, &_windowAttributes);
				_window->rect.x = _windowAttributes.x;
				_window->rect.y = _windowAttributes.y;
			}
			break;
		case ClientMessage:
			_window->event.type = CC_EVENT_WINDOW_QUIT;
			break;
		case EnterNotify:
			_window->event.type = CC_EVENT_FOCUS_GAINED;
			break;
		case LeaveNotify:
			_window->event.type = CC_EVENT_FOCUS_LOST;
			break;
		case FocusIn:
			_window->event.type = CC_EVENT_FOCUS_GAINED;
			break;
		case FocusOut:
			_window->event.type = CC_EVENT_FOCUS_LOST;
			break;
	}

	return true;
}

void ccChangeWM(ccWindowMode mode)
{
	XEvent event;
	XWindowAttributes windowAttributes;
	Atom wmState, fullscreen;
	int windowWidth, windowHeight;

	ccAssert(_window);

	if(mode == CC_WINDOW_MODE_FULLSCREEN || mode == CC_WINDOW_MODE_WINDOW){
		wmState = XInternAtom(_window->XDisplay, "_NET_WM_STATE", false);
		fullscreen = XInternAtom(_window->XDisplay, "_NET_WM_STATE_FULLSCREEN", false);

		memset(&event, 0, sizeof(event));
		event.type = ClientMessage;
		event.xclient.window = _window->XWindow;
		event.xclient.message_type = wmState;
		event.xclient.format = 32;
		event.xclient.data.l[0] = mode == CC_WINDOW_MODE_FULLSCREEN;
		event.xclient.data.l[1] = fullscreen;

		XSendEvent(_window->XDisplay, DefaultRootWindow(_window->XDisplay), false, SubstructureNotifyMask, &event);

		if(mode == CC_WINDOW_MODE_FULLSCREEN){
			XGrabPointer(_window->XDisplay, _window->XWindow, True, 0, GrabModeAsync, GrabModeAsync, _window->XWindow, None, CurrentTime);
		}else{
			XUngrabPointer(_window->XDisplay, CurrentTime);
		}
	}else if(mode == CC_WINDOW_MODE_MAXIMIZED){
		XGetWindowAttributes(_window->XDisplay, DefaultRootWindow(_window->XDisplay), &windowAttributes);
		windowWidth = windowAttributes.width - windowAttributes.x - (windowAttributes.border_width << 1);
		windowHeight = windowAttributes.height - windowAttributes.y - (windowAttributes.border_width << 1);
		XMoveResizeWindow(_window->XDisplay, _window->XWindow, 0, 0, windowWidth, windowHeight);

		XUngrabPointer(_window->XDisplay, CurrentTime);
	}
}

void ccResizeMoveWindow(ccRect rect)
{
	ccAssert(_window);

	XMoveResizeWindow(_window->XDisplay, _window->XWindow, rect.x, rect.y, rect.width, rect.height);
}

void ccCenterWindow()
{
	//TODO send _NET_WM_WINDOW_TYPE_SPLASH event
	XEvent event;
	Atom wmState, splash;

	ccAssert(_window);

	wmState = XInternAtom(_window->XDisplay, "_NET_WM_TYPE", false);
	splash = XInternAtom(_window->XDisplay, "_NET_WM_TYPE_SPLASH", false);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = _window->XWindow;
	event.xclient.message_type = wmState;
	event.xclient.format = 32;
	event.xclient.data.l[0] = 0;
	event.xclient.data.l[1] = splash;

	XSendEvent(_window->XDisplay, DefaultRootWindow(_window->XDisplay), false, SubstructureNotifyMask, &event);
}

ccRect ccGetDisplayRect(ccDisplay *display)
{
	ccRect rect;

	rect.x = display->x;
	rect.y = display->y;
	rect.width = display->resolution[display->current].width;
	rect.height = display->resolution[display->current].height;

	return rect;
}
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

static ccError setWindowState(const char *type, bool value)
{	
	XEvent event;
	Atom wmState, newWmState;

	wmState = XInternAtom(_window->XDisplay, "_NET_WM_STATE", false);
	newWmState = XInternAtom(_window->XDisplay, type, false);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = _window->XWindow;
	event.xclient.message_type = wmState;
	event.xclient.format = 32;
	event.xclient.data.l[0] = value;
	event.xclient.data.l[1] = newWmState;

	XSendEvent(_window->XDisplay, DefaultRootWindow(_window->XDisplay), false, SubstructureNotifyMask, &event);

	return CC_ERROR_NONE;
}

ccError ccNewWindow(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

	ccAssert(_window == NULL);

	ccMalloc(_window, sizeof(ccWindow));
	if(_window == NULL){
		return CC_ERROR_OUT_OF_MEMORY;
	}
	ccAssert(_window != NULL);

	_window->XDisplay = XOpenDisplay(NULL);
	ccAssert(_window->XDisplay != NULL);

	root = DefaultRootWindow(_window->XDisplay);
	_window->XScreen = DefaultScreen(_window->XDisplay);
	_window->XWindow = XCreateSimpleWindow(_window->XDisplay, root, rect.x, rect.y, rect.width, rect.height, 1, 0, 0);
	// Choose types of events
	XSelectInput(_window->XDisplay, _window->XWindow, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask);

	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP){
		setWindowState("_NET_WM_STATE_ABOVE", true);
	}

	XMapWindow(_window->XDisplay, _window->XWindow);
	XStoreName(_window->XDisplay, _window->XWindow, title);

	_window->mouse.x = _window->mouse.y = _window->rect.width = _window->rect.height = -1;

	delete = XInternAtom(_window->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(_window->XDisplay, _window->XWindow, &delete, 1);

	return CC_ERROR_NONE;
}

ccError ccFreeWindow()
{
	ccAssert(_window != NULL);	

	XUnmapWindow(_window->XDisplay, _window->XWindow);
	XCloseDisplay(_window->XDisplay);

	free(_window);
	_window = NULL;

	return CC_ERROR_NONE;
}

bool ccPollEvent()
{
	XEvent event;
	XWindowAttributes _windowAttributes;

	if(!_window){
		return false;
	}

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
			//TODO: ignore undefined
			_window->event.type = CC_EVENT_KEY_DOWN;
			_window->event.keyCode = XLookupKeysym(&event.xkey, 0);
			break;
		case KeyRelease:
			_window->event.type = CC_EVENT_KEY_UP;
			_window->event.keyCode = XLookupKeysym(&event.xkey, 0);
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

ccError ccSetWindowed()
{
	ccAssert(_window);

	setWindowState("_NET_WM_STATE_FULLSCREEN", false);

	//TODO add check for the pointer
	XUngrabPointer(_window->XDisplay, CurrentTime);

	return CC_ERROR_NONE;
}

ccError ccSetMaximized()
{
	XWindowAttributes windowAttributes;
	int windowWidth, windowHeight;

	ccAssert(_window);

	ccSetWindowed();

	XGetWindowAttributes(_window->XDisplay, DefaultRootWindow(_window->XDisplay), &windowAttributes);
	windowWidth = windowAttributes.width - windowAttributes.x - (windowAttributes.border_width << 1);
	windowHeight = windowAttributes.height - windowAttributes.y - (windowAttributes.border_width << 1);
	XMoveResizeWindow(_window->XDisplay, _window->XWindow, 0, 0, windowWidth, windowHeight);

	//setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", true);

	return CC_ERROR_NONE;
}

ccError ccSetFullscreen(int displayCount, ...)
{
	ccAssert(_window);

	//TODO implement multiple displays
	setWindowState("_NET_WM_STATE_FULLSCREEN", true);

	//TODO add check for the pointer
	XGrabPointer(_window->XDisplay, _window->XWindow, True, 0, GrabModeAsync, GrabModeAsync, _window->XWindow, None, CurrentTime);

	return CC_ERROR_NONE;
}

ccError ccResizeMoveWindow(ccRect rect, bool addBorder)
{
	//TODO implement addBorder
	ccAssert(_window);

	XMoveResizeWindow(_window->XDisplay, _window->XWindow, rect.x, rect.y, rect.width, rect.height);

	return CC_ERROR_NONE;
}

ccError ccCenterWindow()
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

	return CC_ERROR_NONE;
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

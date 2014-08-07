#include "x11_window.h"

static ccError setWindowState(const char *type, bool value)
{	
	XEvent event;
	Atom wmState, newWmState;

	wmState = XInternAtom(WINDOW_DATA->XDisplay, "_NET_WM_STATE", false);
	newWmState = XInternAtom(WINDOW_DATA->XDisplay, type, false);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = WINDOW_DATA->XWindow;
	event.xclient.message_type = wmState;
	event.xclient.format = 32;
	event.xclient.data.l[0] = value;
	event.xclient.data.l[1] = newWmState;

	XSendEvent(WINDOW_DATA->XDisplay, DefaultRootWindow(WINDOW_DATA->XDisplay), false, SubstructureNotifyMask, &event);

	return CC_ERROR_NONE;
}

static void setResizable(bool resizable)
{
	XSizeHints *sizeHints = XAllocSizeHints();
	long flags = 0;

	ccAssert(_window != NULL);

	if(WINDOW_DATA->resizable == resizable) return;

	WINDOW_DATA->resizable = resizable;

	XGetWMNormalHints(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, sizeHints, &flags);

	if(resizable) {
		sizeHints->flags &= ~(PMinSize | PMaxSize);
	}else{
		sizeHints->flags |= PMinSize | PMaxSize;
		sizeHints->min_width = sizeHints->max_width = _window->rect.width;
		sizeHints->min_height = sizeHints->max_height = _window->rect.height;
	}

	XSetWMNormalHints(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, sizeHints);

	XFree(sizeHints);
}

ccError ccNewWindow(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

	ccAssert(_window == NULL);

	ccMalloc(_window, sizeof(ccWindow));
	
	_window->rect = rect;
	ccMalloc(_window->data, sizeof(ccWindow_x11));
	WINDOW_DATA->windowFlags = flags;

	WINDOW_DATA->XDisplay = XOpenDisplay(NULL);
	if(WINDOW_DATA->XDisplay == NULL) return CC_ERROR_WINDOWCREATION;

	root = DefaultRootWindow(WINDOW_DATA->XDisplay);
	WINDOW_DATA->XScreen = DefaultScreen(WINDOW_DATA->XDisplay);
	WINDOW_DATA->XWindow = XCreateSimpleWindow(WINDOW_DATA->XDisplay, root, rect.x, rect.y, rect.width, rect.height, 1, 0, 0);

	// Choose types of events
	XSelectInput(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask);

	// Disable resizing
	WINDOW_DATA->resizable = true;
	if(flags & CC_WINDOW_FLAG_NORESIZE) setResizable(false);

	if(flags & CC_WINDOW_FLAG_ALWAYSONTOP){
		setWindowState("_NET_WM_STATE_ABOVE", true);
	}

	XMapWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);
	XStoreName(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, title);

	delete = XInternAtom(WINDOW_DATA->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &delete, 1);

	ccUpdateWindowDisplay();

	return CC_ERROR_NONE;
}

ccError ccFreeWindow()
{
	ccAssert(_window != NULL);	

	XUnmapWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);
	XCloseDisplay(WINDOW_DATA->XDisplay);

	free(_window->data);
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
	if(XPending(WINDOW_DATA->XDisplay) == 0){
		return false;
	}

	XNextEvent(WINDOW_DATA->XDisplay, &event);
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
		case ConfigureNotify: //NOTE: only fires after polling
			if(_window->rect.width != event.xconfigure.width || _window->rect.height != event.xconfigure.height){
				_window->event.type = CC_EVENT_WINDOW_RESIZE;
				_window->rect.width = event.xconfigure.width;
				_window->rect.height = event.xconfigure.height;
				_window->aspect = _window->rect.height / _window->rect.width;

				XGetWindowAttributes(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &_windowAttributes);

				_window->rect.x = _windowAttributes.x;
				_window->rect.y = _windowAttributes.y;

				ccUpdateWindowDisplay(); //TODO: also do this when moving the window

				if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE) setResizable(false);

				printf("Changed to %dx%d\n", _window->rect.width, _window->rect.height);
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

	setResizable(true);

	setWindowState("_NET_WM_STATE_FULLSCREEN", false);

	//TODO add check for the pointer
	XUngrabPointer(WINDOW_DATA->XDisplay, CurrentTime);

	return CC_ERROR_NONE;
}

ccError ccSetMaximized()
{
	ccAssert(_window);

	ccSetWindowed();

	setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", true);
	setWindowState("_NET_WM_STATE_MAXIMIZED_HORZ", true);

	return CC_ERROR_NONE;
}

ccError ccSetFullscreen(int displayCount, ...)
{
	ccAssert(_window);

	setResizable(true);

	//TODO implement multiple displays
	setWindowState("_NET_WM_STATE_FULLSCREEN", true);

	//TODO add check for the pointer
	XGrabPointer(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, True, 0, GrabModeAsync, GrabModeAsync, WINDOW_DATA->XWindow, None, CurrentTime);

	return CC_ERROR_NONE;
}

ccError ccResizeMoveWindow(ccRect rect, bool addBorder)
{
	//TODO implement addBorder
	ccAssert(_window);

	setResizable(true);
	XMoveResizeWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, rect.x, rect.y, rect.width, rect.height);
	//TODO prime resize event here
	_window->rect = rect;
	if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE) setResizable(false);


	return CC_ERROR_NONE;
}

ccError ccCenterWindow()
{
	ccDisplayData *currentResolution;
	ccRect newRect;

	ccAssert(_window->display != NULL);

	currentResolution = ccGetResolutionCurrent(_window->display);

	newRect.x = (currentResolution->width - _window->rect.width) >> 1;
	newRect.y = (currentResolution->height - _window->rect.height) >> 1;
	newRect.width = _window->rect.width;
	newRect.height = _window->rect.height;

	ccResizeMoveWindow(newRect, true);

	return CC_ERROR_NONE;
}

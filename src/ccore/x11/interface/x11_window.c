#include "x11_window.h"

static ccError setWindowState(const char *type, bool value)
{	
	XEvent event;
	Atom wmState, newWmState;

	wmState = XInternAtom(WINDOW_DATA->XDisplay, "_NET_WM_STATE", 1);
	newWmState = XInternAtom(WINDOW_DATA->XDisplay, type, 1);

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
	XSizeHints *sizeHints;
	long flags;

	ccAssert(_window != NULL);

	sizeHints = XAllocSizeHints();
	flags = 0;

	if(WINDOW_DATA->resizable == resizable){
		XFree(sizeHints);
		return;
	}

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

bool checkRawSupport()
{
	int event, error, mayor, minor;

	if(!XQueryExtension(WINDOW_DATA->XDisplay, "XInputExtension", &WINDOW_DATA->XInputOpcode, &event, &error)){
		return false;
	}

	mayor = 2;
	minor = 0;
	if(XIQueryVersion(WINDOW_DATA->XDisplay, &mayor, &minor) == BadRequest){
		return false;
	}

	return true;
}

void initRawSupport()
{
	XIEventMask mask;

	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = XIMaskLen(XI_RawMotion);
	mask.mask = calloc(mask.mask_len, sizeof(char));

	XISetMask(mask.mask, XI_Enter);
	XISetMask(mask.mask, XI_Leave);
	XISetMask(mask.mask, XI_ButtonPress);
	XISetMask(mask.mask, XI_ButtonRelease);
	XISetMask(mask.mask, XI_KeyPress);
	XISetMask(mask.mask, XI_KeyRelease);

	XISelectEvents(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &mask, 1);

	mask.deviceid = XIAllDevices;
	memset(mask.mask, 0, mask.mask_len);
	XISetMask(mask.mask, XI_RawMotion);
	XISetMask(mask.mask, XI_RawButtonPress);
	XISetMask(mask.mask, XI_RawButtonRelease);
	XISetMask(mask.mask, XI_RawKeyPress);
	XISetMask(mask.mask, XI_RawKeyRelease);

	XISelectEvents(WINDOW_DATA->XDisplay, DefaultRootWindow(WINDOW_DATA->XDisplay), &mask, 1);

	free(mask.mask);
}

ccPoint getRawMouseMovement(XIRawEvent *event)
{
	ccPoint delta;
	double *rawValuator, *valuator;
	int i;

	delta.x = delta.y = 0;
	rawValuator = event->raw_values;
	valuator = event->valuators.values;

	for(i = 0; i < event->valuators.mask_len * 8; i++){
		if(XIMaskIsSet(event->valuators.mask, i)){
			if(i == 0){
				delta.x += *valuator - *rawValuator;
			}else if(i == 1){
				delta.y += *valuator - *rawValuator;
			}
			valuator++;
			rawValuator++;
		}
	}

	return delta;
}

ccError ccWindowCreate(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

	ccAssert(_window == NULL);

	ccMalloc(_window, sizeof(ccWindow));

	_window->rect = rect;
	ccMalloc(_window->data, sizeof(ccWindow_x11));
	WINDOW_DATA->windowFlags = flags;

	WINDOW_DATA->XDisplay = XOpenDisplay(NULL);
	if(WINDOW_DATA->XDisplay == NULL){
		return CC_ERROR_WINDOWCREATION;
	}

	root = DefaultRootWindow(WINDOW_DATA->XDisplay);
	WINDOW_DATA->XScreen = DefaultScreen(WINDOW_DATA->XDisplay);
	WINDOW_DATA->XWindow = XCreateSimpleWindow(WINDOW_DATA->XDisplay, root, rect.x, rect.y, rect.width, rect.height, 1, 0, 0);

	// Choose types of events
	XSelectInput(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | FocusChangeMask);

	// Disable resizing
	WINDOW_DATA->resizable = true;
	if(flags & CC_WINDOW_FLAG_NORESIZE){
		setResizable(false);
	}

	XMapWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);
	XStoreName(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, title);

	delete = XInternAtom(WINDOW_DATA->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &delete, 1);

	ccWindowUpdateDisplay();

	// Activate always on top if applicable
	if(flags & CC_WINDOW_FLAG_ALWAYSONTOP){
		setWindowState("_NET_WM_STATE_ABOVE", true);
	}

	if(flags & CC_WINDOW_FLAG_PREFERRAW){
		initRawSupport();
		_window->supportsRawInput = checkRawSupport();
	}

	_window->mouse.x = _window->mouse.y = 0;

	return CC_ERROR_NONE;
}

ccError ccWindowFree(void)
{
	ccAssert(_window != NULL);	

	XUnmapWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);
	XCloseDisplay(WINDOW_DATA->XDisplay);

	free(_window->data);
	free(_window);
	_window = NULL;

	return CC_ERROR_NONE;
}

bool ccWindowPollEvent(void)
{
	XEvent event;
	XWindowAttributes _windowAttributes;
	XGenericEventCookie *cookie;
	ccGamepadEvent gamepadEvent;

	if(!_window){
		return false;
	}

	_window->event.type = CC_EVENT_SKIP;

	gamepadEvent = ccGamepadEventPoll();
	if(gamepadEvent.type != CC_GAMEPAD_UNHANDLED){
		_window->event.type = CC_EVENT_GAMEPAD;
		_window->event.gamepadEvent = gamepadEvent;
		return true;
	}

	if(XPending(WINDOW_DATA->XDisplay) == 0){
		return false;
	}

	XNextEvent(WINDOW_DATA->XDisplay, &event);
	switch(event.type){
		case GenericEvent:
			if(!_window->supportsRawInput){
				return false;
			}

			cookie = &event.xcookie;
			if(!XGetEventData(WINDOW_DATA->XDisplay, cookie) || 
					cookie->type != GenericEvent ||
					cookie->extension != WINDOW_DATA->XInputOpcode){
				return false;
			}

			switch(cookie->evtype){
				case XI_RawMotion:
					_window->event.type = CC_EVENT_MOUSE_MOVE;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					_window->event.mouseVector = getRawMouseMovement(cookie->data);
					break;
				case XI_RawButtonPress:
					_window->event.type = CC_EVENT_MOUSE_DOWN;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					_window->event.mouseButton = ((XIRawEvent*)cookie->data)->detail;
					break;
				case XI_RawButtonRelease:
					_window->event.type = CC_EVENT_MOUSE_UP;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					_window->event.mouseButton = ((XIRawEvent*)cookie->data)->detail;
					break;
				case XI_Enter:
					_window->event.type = CC_EVENT_FOCUS_GAINED;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					break;
				case XI_Leave:
					_window->event.type = CC_EVENT_FOCUS_LOST;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					break;
				case XI_ButtonPress:
					_window->event.type = CC_EVENT_MOUSE_DOWN;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					_window->event.mouseButton = event.xbutton.button;
					break;
				case XI_ButtonRelease:
					_window->event.type = CC_EVENT_MOUSE_UP;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					_window->event.mouseButton = event.xbutton.button;
					break;
				case XI_KeyPress:
					_window->event.type = CC_EVENT_KEY_DOWN;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					break;
				case XI_KeyRelease:
					_window->event.type = CC_EVENT_KEY_UP;
					_window->event.deviceId = ((XIDeviceEvent*)cookie->data)->deviceid;
					break;
			}
			break;
		case ButtonPress:
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
			if(_window->mouse.x != event.xmotion.x ||
					_window->mouse.y != event.xmotion.y){
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
			_window->event.keyCode = XLookupKeysym(&event.xkey, 0);
			break;
		case KeyRelease:
			_window->event.type = CC_EVENT_KEY_UP;
			_window->event.keyCode = XLookupKeysym(&event.xkey, 0);
			break;
		case ConfigureNotify:
			if(_window->rect.width != event.xconfigure.width ||
					_window->rect.height != event.xconfigure.height){
				_window->event.type = CC_EVENT_WINDOW_RESIZE;
				_window->rect.width = event.xconfigure.width;
				_window->rect.height = event.xconfigure.height;
				_window->aspect = _window->rect.height / _window->rect.width;

				XGetWindowAttributes(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &_windowAttributes);

				_window->rect.x = _windowAttributes.x;
				_window->rect.y = _windowAttributes.y;

				ccWindowUpdateDisplay();

				if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
					setResizable(false);
				}
			}

			if(_window->rect.x != event.xconfigure.x ||
					_window->rect.y != event.xconfigure.y) {
				_window->rect.x = event.xconfigure.x;
				_window->rect.y = event.xconfigure.y;

				ccWindowUpdateDisplay();
			}
			break;
		case ClientMessage:
			_window->event.type = CC_EVENT_WINDOW_QUIT;
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

ccError ccWindowSetWindowed(void)
{
	ccAssert(_window);

	setResizable(true);
	setWindowState("_NET_WM_STATE_FULLSCREEN", false);
	setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", false);
	setWindowState("_NET_WM_STATE_MAXIMIZED_HORZ", false);

	return CC_ERROR_NONE;
}

ccError ccWindowSetMaximized(void)
{
	ccAssert(_window);

	ccWindowSetWindowed();

	setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", true);
	setWindowState("_NET_WM_STATE_MAXIMIZED_HORZ", true);

	return CC_ERROR_NONE;
}

ccError ccWindowSetFullscreen(int displayCount, ...)
{
	XEvent event;
	Atom atom;
	va_list displays;
	int i;
	ccDisplay *current, *topDisplay, *bottomDisplay, *leftDisplay, *rightDisplay;

	ccAssert(_window);

	if(displayCount == CC_FULLSCREEN_CURRENT_DISPLAY) {
		topDisplay = bottomDisplay = leftDisplay = rightDisplay = _window->display;
	}else{
		va_start(displays, displayCount);

		topDisplay = bottomDisplay = leftDisplay = rightDisplay = va_arg(displays, ccDisplay*);

		for(i=1;i<displayCount;i++) {
			current = va_arg(displays, ccDisplay*);

			if(current->x < leftDisplay->x){
				leftDisplay = current;
			}
			if(current->y < topDisplay->y){
				topDisplay = current;
			}
			if(current->x + ccDisplayGetResolutionCurrent(current)->width > rightDisplay->x + ccDisplayGetResolutionCurrent(rightDisplay)->width){ 
				rightDisplay = current;
			}
			if(current->y + ccDisplayGetResolutionCurrent(current)->height > bottomDisplay->y + ccDisplayGetResolutionCurrent(bottomDisplay)->width){ 
				bottomDisplay = current;
			}
		}

		va_end(displays);
	}

	atom = XInternAtom(WINDOW_DATA->XDisplay, "_NET_WM_FULLSCREEN_MONITORS", False);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = WINDOW_DATA->XWindow;
	event.xclient.message_type = atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = DISPLAY_DATA(topDisplay)->XineramaScreen;
	event.xclient.data.l[1] = DISPLAY_DATA(bottomDisplay)->XineramaScreen;
	event.xclient.data.l[2] = DISPLAY_DATA(leftDisplay)->XineramaScreen;
	event.xclient.data.l[3] = DISPLAY_DATA(rightDisplay)->XineramaScreen;
	event.xclient.data.l[4] = 0;

	XSendEvent(WINDOW_DATA->XDisplay, DefaultRootWindow(WINDOW_DATA->XDisplay), false, SubstructureNotifyMask, &event);

	setResizable(true);
	setWindowState("_NET_WM_STATE_FULLSCREEN", true);

	return CC_ERROR_NONE;
}

ccError ccWindowResizeMove(ccRect rect)
{
	ccAssert(_window);

	setResizable(true);
	XMoveResizeWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, rect.x, rect.y, rect.width, rect.height);

	_window->rect = rect;
	if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
		setResizable(false);
	}

	return CC_ERROR_NONE;
}

ccError ccWindowCenter(void)
{
	ccDisplayData *currentResolution;
	ccRect newRect;

	ccAssert(_window->display != NULL);

	currentResolution = ccDisplayGetResolutionCurrent(_window->display);

	newRect.x = (currentResolution->width - _window->rect.width) >> 1;
	newRect.y = (currentResolution->height - _window->rect.height) >> 1;
	newRect.width = _window->rect.width;
	newRect.height = _window->rect.height;

	ccWindowResizeMove(newRect);

	return CC_ERROR_NONE;
}

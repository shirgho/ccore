#include "x11_window.h"

static int cursorList[] =
{
	XC_arrow,
	XC_crosshair,
	XC_xterm,
	XC_fleur,
	XC_hand1,
	XC_sb_h_double_arrow,
	XC_sb_v_double_arrow,
	XC_X_cursor,
	XC_question_arrow
};

static ccReturn setWindowState(const char *type, bool value)
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

	return CC_SUCCESS;
}

static ccReturn setResizable(bool resizable)
{
	XSizeHints *sizeHints;
	long flags;

	ccAssert(_ccWindow != NULL);

	sizeHints = XAllocSizeHints();
	flags = 0;

	if(WINDOW_DATA->resizable == resizable){
		XFree(sizeHints);
		return CC_SUCCESS;
	}

	WINDOW_DATA->resizable = resizable;

	XGetWMNormalHints(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, sizeHints, &flags);

	if(resizable) {
		sizeHints->flags &= ~(PMinSize | PMaxSize);
	}else{
		sizeHints->flags |= PMinSize | PMaxSize;
		sizeHints->min_width = sizeHints->max_width = _ccWindow->rect.width;
		sizeHints->min_height = sizeHints->max_height = _ccWindow->rect.height;
	}

	XSetWMNormalHints(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, sizeHints);

	XFree(sizeHints);

	return CC_SUCCESS;
}

static ccReturn checkRawSupport()
{
	int event, error, mayor, minor;

	if(CC_UNLIKELY(!XQueryExtension(WINDOW_DATA->XDisplay, "XInputExtension", &WINDOW_DATA->XInputOpcode, &event, &error))){
		return CC_FAIL;
	}

	mayor = 2;
	minor = 0;
	if(CC_UNLIKELY(XIQueryVersion(WINDOW_DATA->XDisplay, &mayor, &minor) == BadRequest)){
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

static ccReturn initRawSupport()
{
	XIEventMask mask;

	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = XIMaskLen(XI_RawMotion);
	ccCalloc(mask.mask, mask.mask_len, sizeof(char));

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

	return CC_SUCCESS;
}

static ccPoint getRawMouseMovement(XIRawEvent *event)
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

static inline unsigned int getRawKeyboardCode(XIRawEvent *event)
{
	return XGetKeyboardMapping(WINDOW_DATA->XDisplay, event->detail, 1, (int[]){1})[0];
}

ccReturn ccWindowCreate(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

	if(CC_UNLIKELY(_ccWindow != NULL)){
		ccErrorPush(CC_ERROR_WINDOW_CREATE);
		return CC_FAIL;
	}

	ccMalloc(_ccWindow, sizeof(ccWindow));

	_ccWindow->rect = rect;
	ccMalloc(_ccWindow->data, sizeof(ccWindow_x11));
	WINDOW_DATA->windowFlags = flags;

	WINDOW_DATA->XDisplay = XOpenDisplay(NULL);
	if(CC_UNLIKELY(WINDOW_DATA->XDisplay == NULL)){
		ccErrorPush(CC_ERROR_WINDOW_CREATE);
		return CC_FAIL;
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

	if((flags & CC_WINDOW_FLAG_NORAWINPUT) != 0){
		_ccWindow->supportsRawInput = checkRawSupport();
		if(_ccWindow->supportsRawInput){
			initRawSupport();
		}
	}else{
		_ccWindow->supportsRawInput = false;
	}

	_ccWindow->mouse.x = _ccWindow->mouse.y = 0;

	return CC_SUCCESS;
}

ccReturn ccWindowFree(void)
{
	ccAssert(_ccWindow);

	XFreeCursor(WINDOW_DATA->XDisplay, WINDOW_DATA->XCursor);
	XUnmapWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);
	XCloseDisplay(WINDOW_DATA->XDisplay);

	free(_ccWindow->data);
	free(_ccWindow);
	_ccWindow = NULL;

	return CC_SUCCESS;
}

bool ccWindowPollEvent(void)
{
	XEvent event;
	XWindowAttributes _ccWindowAttributes;
	XGenericEventCookie *cookie;
	ccGamepadEvent gamepadEvent;

	ccAssert(_ccWindow);

	_ccWindow->event.type = CC_EVENT_SKIP;

	if(_ccGamepads != NULL){
		gamepadEvent = ccGamepadEventPoll();
		if(gamepadEvent.type != CC_GAMEPAD_UNHANDLED){
			_ccWindow->event.type = CC_EVENT_GAMEPAD;
			_ccWindow->event.gamepadEvent = gamepadEvent;
			return true;
		}
	}

	if(XPending(WINDOW_DATA->XDisplay) == 0){
		return false;
	}

	XNextEvent(WINDOW_DATA->XDisplay, &event);
	switch(event.type){
		case GenericEvent:
			if(CC_UNLIKELY(!_ccWindow->supportsRawInput)){
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
					_ccWindow->event.type = CC_EVENT_MOUSE_MOVE;
					_ccWindow->event.mouseDelta = getRawMouseMovement(cookie->data);
					break;
				case XI_RawButtonPress:
					_ccWindow->event.type = CC_EVENT_MOUSE_DOWN;
					_ccWindow->event.mouseButton = ((XIRawEvent*)cookie->data)->detail;
					break;
				case XI_RawButtonRelease:
					_ccWindow->event.type = CC_EVENT_MOUSE_UP;
					_ccWindow->event.mouseButton = ((XIRawEvent*)cookie->data)->detail;
					break;
				case XI_RawKeyPress:
					_ccWindow->event.type = CC_EVENT_KEY_DOWN;
					_ccWindow->event.keyCode = getRawKeyboardCode(cookie->data);
					break;
				case XI_RawKeyRelease:
					_ccWindow->event.type = CC_EVENT_KEY_UP;
					_ccWindow->event.keyCode = getRawKeyboardCode(cookie->data);
					break;
				case XI_Enter:
					_ccWindow->event.type = CC_EVENT_FOCUS_GAINED;
					break;
				case XI_Leave:
					_ccWindow->event.type = CC_EVENT_FOCUS_LOST;
					break;
			}
			break;
		case ButtonPress:
			if(event.xbutton.button <= 3){
				_ccWindow->event.type = CC_EVENT_MOUSE_DOWN;
				_ccWindow->event.mouseButton = event.xbutton.button;
			}else if(event.xbutton.button == 4){
				_ccWindow->event.type = CC_EVENT_MOUSE_SCROLL;
				_ccWindow->event.scrollDelta = 1;
			}else if(event.xbutton.button == 5){
				_ccWindow->event.type = CC_EVENT_MOUSE_SCROLL;
				_ccWindow->event.scrollDelta = -1;
			}
			break;
		case ButtonRelease:
			_ccWindow->event.type = CC_EVENT_MOUSE_UP;
			_ccWindow->event.mouseButton = event.xbutton.button;
			break;
		case MotionNotify:
			if(CC_UNLIKELY(!_ccWindow->supportsRawInput)){
				_ccWindow->event.type = CC_EVENT_MOUSE_MOVE;
				_ccWindow->event.mouseDelta.x = _ccWindow->mouse.x - event.xmotion.x;
				_ccWindow->event.mouseDelta.y = _ccWindow->mouse.y - event.xmotion.y;
			}
			if(CC_LIKELY(_ccWindow->mouse.x != event.xmotion.x ||
					_ccWindow->mouse.y != event.xmotion.y)){
				_ccWindow->event.type = CC_EVENT_MOUSE_MOVE;
				_ccWindow->mouse.x = event.xmotion.x;
				_ccWindow->mouse.y = event.xmotion.y;
			}
			break;
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;
		case KeyPress:
			_ccWindow->event.type = CC_EVENT_KEY_DOWN;
			_ccWindow->event.keyCode = XLookupKeysym(&event.xkey, 0);
			break;
		case KeyRelease:
			_ccWindow->event.type = CC_EVENT_KEY_UP;
			_ccWindow->event.keyCode = XLookupKeysym(&event.xkey, 0);
			break;
		case ConfigureNotify:
			if(_ccWindow->rect.width != event.xconfigure.width ||
					_ccWindow->rect.height != event.xconfigure.height){
				_ccWindow->event.type = CC_EVENT_WINDOW_RESIZE;
				_ccWindow->rect.width = event.xconfigure.width;
				_ccWindow->rect.height = event.xconfigure.height;

				XGetWindowAttributes(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, &_ccWindowAttributes);

				_ccWindow->rect.x = _ccWindowAttributes.x;
				_ccWindow->rect.y = _ccWindowAttributes.y;

				ccWindowUpdateDisplay();

				if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
					setResizable(false);
				}
			}

			if(_ccWindow->rect.x != event.xconfigure.x ||
					_ccWindow->rect.y != event.xconfigure.y) {
				_ccWindow->rect.x = event.xconfigure.x;
				_ccWindow->rect.y = event.xconfigure.y;

				ccWindowUpdateDisplay();
			}
			break;
		case ClientMessage:
			_ccWindow->event.type = CC_EVENT_WINDOW_QUIT;
			break;
		case FocusIn:
			_ccWindow->event.type = CC_EVENT_FOCUS_GAINED;
			break;
		case FocusOut:
			_ccWindow->event.type = CC_EVENT_FOCUS_LOST;
			break;
	}

	return true;
}

ccReturn ccWindowSetWindowed(void)
{	
	ccAssert(_ccWindow);

	setResizable(true);
	setWindowState("_NET_WM_STATE_FULLSCREEN", false);
	setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", false);
	setWindowState("_NET_WM_STATE_MAXIMIZED_HORZ", false);

	return CC_SUCCESS;
}

ccReturn ccWindowSetMaximized(void)
{
	ccAssert(_ccWindow);

	ccWindowSetWindowed();

	setWindowState("_NET_WM_STATE_MAXIMIZED_VERT", true);
	setWindowState("_NET_WM_STATE_MAXIMIZED_HORZ", true);

	return CC_SUCCESS;
}

ccReturn ccWindowSetFullscreen(int displayCount, ...)
{
	XEvent event;
	Atom atom;
	va_list displays;
	int i;
	ccDisplay *current, *topDisplay, *bottomDisplay, *leftDisplay, *rightDisplay;

	ccAssert(_ccWindow);

	if(CC_LIKELY(displayCount == CC_FULLSCREEN_CURRENT_DISPLAY)) {
		topDisplay = bottomDisplay = leftDisplay = rightDisplay = _ccWindow->display;
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

	return CC_SUCCESS;
}

ccReturn ccWindowBlink(void)
{
	ccAssert(_ccWindow);

	return setWindowState("_NET_WM_STATE_DEMANDS_ATTENTION", true);
}

ccReturn ccWindowResizeMove(ccRect rect)
{
	ccAssert(_ccWindow);

	setResizable(true);
	XMoveResizeWindow(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, rect.x, rect.y, rect.width, rect.height);

	_ccWindow->rect = rect;
	if(WINDOW_DATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
		setResizable(false);
	}

	ccWindowUpdateDisplay();

	return CC_SUCCESS;
}

ccReturn ccWindowCenter(void)
{
	ccDisplayData *currentResolution;
	ccRect newRect;

	ccAssert(_ccWindow);
	if(CC_UNLIKELY(_ccWindow->display == NULL)){
		ccErrorPush(CC_ERROR_DISPLAY_NONE);
		return CC_FAIL;
	}

	currentResolution = ccDisplayGetResolutionCurrent(_ccWindow->display);

	newRect.x = (currentResolution->width - _ccWindow->rect.width) >> 1;
	newRect.y = (currentResolution->height - _ccWindow->rect.height) >> 1;
	newRect.width = _ccWindow->rect.width;
	newRect.height = _ccWindow->rect.height;

	ccWindowResizeMove(newRect);

	return CC_SUCCESS;
}

ccReturn ccWindowSetMousePosition(ccPoint target)
{
	ccAssert(_ccWindow);

	XWarpPointer(WINDOW_DATA->XDisplay, None, WINDOW_DATA->XWindow, 0, 0, 0, 0, target.x, target.y);

	return CC_SUCCESS;
}

ccReturn ccWindowSetMouseCursor(ccCursor cursor)
{
	ccAssert(_ccWindow);

	if(cursor != CC_CURSOR_NONE){
		WINDOW_DATA->XCursor = XCreateFontCursor(WINDOW_DATA->XDisplay, cursorList[cursor]);
		XDefineCursor(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, WINDOW_DATA->XCursor);
	}

	return CC_SUCCESS;
}

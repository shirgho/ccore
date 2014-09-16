#include "x11_window.h"

static int cursorList[] ={
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

static char emptyCursorData[] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

static ccReturn setWindowState(const char *type, bool value)
{	
	XEvent event;
	Atom wmState, newWmState;

	wmState = XInternAtom(XWINDATA->XDisplay, "_NET_WM_STATE", 1);
	newWmState = XInternAtom(XWINDATA->XDisplay, type, 1);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = XWINDATA->XWindow;
	event.xclient.message_type = wmState;
	event.xclient.format = 32;
	event.xclient.data.l[0] = value;
	event.xclient.data.l[1] = newWmState;

	XSendEvent(XWINDATA->XDisplay, DefaultRootWindow(XWINDATA->XDisplay), false, SubstructureNotifyMask, &event);

	return CC_SUCCESS;
}

static ccReturn setResizable(bool resizable)
{
	XSizeHints *sizeHints;
	long flags;

	ccAssert(_ccWindow != NULL);

	sizeHints = XAllocSizeHints();
	flags = 0;

	if(XWINDATA->resizable == resizable){
		XFree(sizeHints);
		return CC_SUCCESS;
	}

	XWINDATA->resizable = resizable;

	XGetWMNormalHints(XWINDATA->XDisplay, XWINDATA->XWindow, sizeHints, &flags);

	if(resizable) {
		sizeHints->flags &= ~(PMinSize | PMaxSize);
	}else{
		sizeHints->flags |= PMinSize | PMaxSize;
		sizeHints->min_width = sizeHints->max_width = _ccWindow->rect.width;
		sizeHints->min_height = sizeHints->max_height = _ccWindow->rect.height;
	}

	XSetWMNormalHints(XWINDATA->XDisplay, XWINDATA->XWindow, sizeHints);

	XFree(sizeHints);

	return CC_SUCCESS;
}

static ccReturn checkRawSupport()
{
	int event, error, mayor, minor;

	if(CC_UNLIKELY(!XQueryExtension(XWINDATA->XDisplay, "XInputExtension", &XWINDATA->XInputOpcode, &event, &error))){
		return CC_FAIL;
	}

	mayor = 2;
	minor = 0;
	if(CC_UNLIKELY(XIQueryVersion(XWINDATA->XDisplay, &mayor, &minor) == BadRequest)){
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

	XISelectEvents(XWINDATA->XDisplay, XWINDATA->XWindow, &mask, 1);

	mask.deviceid = XIAllDevices;
	memset(mask.mask, 0, mask.mask_len);
	XISetMask(mask.mask, XI_RawMotion);
	XISetMask(mask.mask, XI_RawButtonPress);
	XISetMask(mask.mask, XI_RawButtonRelease);
	XISetMask(mask.mask, XI_RawKeyPress);
	XISetMask(mask.mask, XI_RawKeyRelease);

	XISelectEvents(XWINDATA->XDisplay, DefaultRootWindow(XWINDATA->XDisplay), &mask, 1);

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
	return XGetKeyboardMapping(XWINDATA->XDisplay, event->detail, 1, (int[]){1})[0];
}

static int (*origXError)(Display *display, XErrorEvent *event) = NULL;
static int handleXError(Display *display, XErrorEvent *event)
{
	ccPrintf("X error %d\n", event->error_code);
	
	ccErrorPush(CC_ERROR_WM);
	if(origXError){
		return origXError(display, event);
	}
	return 0;
}

ccReturn ccWindowCreate(ccRect rect, const char *title, int flags)
{
	Atom delete;

	if(CC_UNLIKELY(_ccWindow != NULL)){
		ccErrorPush(CC_ERROR_WINDOW_CREATE);
		return CC_FAIL;
	}

	ccMalloc(_ccWindow, sizeof(ccWindow));

	_ccWindow->rect = rect;
	ccMalloc(_ccWindow->data, sizeof(ccWindow_x11));
	XWINDATA->windowFlags = flags;
	
#ifdef _DEBUG
	origXError = XSetErrorHandler(handleXError);
#endif

	XWINDATA->XDisplay = XOpenDisplay(NULL);
	if(CC_UNLIKELY(XWINDATA->XDisplay == NULL)){
		ccErrorPush(CC_ERROR_WINDOW_CREATE);
		return CC_FAIL;
	}

	XWINDATA->XScreen = DefaultScreen(XWINDATA->XDisplay);
	XWINDATA->XNetIcon = XInternAtom(XWINDATA->XDisplay, "_NET_WM_ICON", False);
	XWINDATA->XWindow = XCreateWindow(XWINDATA->XDisplay, RootWindow(XWINDATA->XDisplay, XWINDATA->XScreen), rect.x, rect.y, rect.width, rect.height, 0, CopyFromParent, InputOutput, CopyFromParent, 0, 0);

	// Choose types of events
	XSelectInput(XWINDATA->XDisplay, XWINDATA->XWindow, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | FocusChangeMask);

	// Disable resizing
	XWINDATA->resizable = true;
	if(flags & CC_WINDOW_FLAG_NORESIZE){
		setResizable(false);
	}

	XMapWindow(XWINDATA->XDisplay, XWINDATA->XWindow);
	XStoreName(XWINDATA->XDisplay, XWINDATA->XWindow, title);

	delete = XInternAtom(XWINDATA->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(XWINDATA->XDisplay, XWINDATA->XWindow, &delete, 1);

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
	XWINDATA->XCursor = 0;
	XWINDATA->XEmptyCursorImage = XCreateBitmapFromData(XWINDATA->XDisplay, XWINDATA->XWindow, emptyCursorData, 8, 8);

	return CC_SUCCESS;
}

ccReturn ccWindowFree(void)
{
	ccAssert(_ccWindow);

	if(XWINDATA->XCursor != 0){
		XFreeCursor(XWINDATA->XDisplay, XWINDATA->XCursor);
	}
	XFreePixmap(XWINDATA->XDisplay, XWINDATA->XEmptyCursorImage);
	XUnmapWindow(XWINDATA->XDisplay, XWINDATA->XWindow);
	XCloseDisplay(XWINDATA->XDisplay);

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

	if(XPending(XWINDATA->XDisplay) == 0){
		return false;
	}

	XNextEvent(XWINDATA->XDisplay, &event);
	switch(event.type){
		case GenericEvent:
			if(CC_UNLIKELY(!_ccWindow->supportsRawInput)){
				return false;
			}

			cookie = &event.xcookie;
			if(!XGetEventData(XWINDATA->XDisplay, cookie) || 
					cookie->type != GenericEvent ||
					cookie->extension != XWINDATA->XInputOpcode){
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

				XGetWindowAttributes(XWINDATA->XDisplay, XWINDATA->XWindow, &_ccWindowAttributes);

				_ccWindow->rect.x = _ccWindowAttributes.x;
				_ccWindow->rect.y = _ccWindowAttributes.y;

				ccWindowUpdateDisplay();

				if(XWINDATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
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

	atom = XInternAtom(XWINDATA->XDisplay, "_NET_WM_FULLSCREEN_MONITORS", False);

	memset(&event, 0, sizeof(event));
	event.type = ClientMessage;
	event.xclient.window = XWINDATA->XWindow;
	event.xclient.message_type = atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = DISPLAY_DATA(topDisplay)->XineramaScreen;
	event.xclient.data.l[1] = DISPLAY_DATA(bottomDisplay)->XineramaScreen;
	event.xclient.data.l[2] = DISPLAY_DATA(leftDisplay)->XineramaScreen;
	event.xclient.data.l[3] = DISPLAY_DATA(rightDisplay)->XineramaScreen;
	event.xclient.data.l[4] = 0;

	XSendEvent(XWINDATA->XDisplay, DefaultRootWindow(XWINDATA->XDisplay), false, SubstructureNotifyMask, &event);

	setResizable(true);
	setWindowState("_NET_WM_STATE_FULLSCREEN", true);

	return CC_SUCCESS;
}

ccReturn ccWindowResizeMove(ccRect rect)
{
	ccAssert(_ccWindow);

	setResizable(true);
	XMoveResizeWindow(XWINDATA->XDisplay, XWINDATA->XWindow, rect.x, rect.y, rect.width, rect.height);

	_ccWindow->rect = rect;
	if(XWINDATA->windowFlags & CC_WINDOW_FLAG_NORESIZE){
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

ccReturn ccWindowBlink(void)
{
	ccAssert(_ccWindow);

	return setWindowState("_NET_WM_STATE_DEMANDS_ATTENTION", true);
}

ccReturn ccWindowSetIcon(ccPoint size, unsigned long *icon)
{
	unsigned long *data;
	size_t dataLen, totalLen;

	ccAssert(_ccWindow);

	if(size.x <= 0 || size.y <= 0){
		ccErrorPush(CC_ERROR_WINDOW_MODE);
		return CC_FAIL;
	}

	dataLen = size.x * size.y;
	totalLen = dataLen + 2;
	ccMalloc(data, totalLen * sizeof(unsigned long));
	
	data[0] = (unsigned long)size.x;
	data[1] = (unsigned long)size.y;
	memcpy(data + 2, icon, dataLen * sizeof(unsigned long));

	XChangeProperty(XWINDATA->XDisplay, XWINDATA->XWindow, XWINDATA->XNetIcon, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, totalLen); 

	free(data);

	return CC_SUCCESS;
}

ccReturn ccWindowSetMousePosition(ccPoint target)
{
	ccAssert(_ccWindow);

	XWarpPointer(XWINDATA->XDisplay, None, XWINDATA->XWindow, 0, 0, 0, 0, target.x, target.y);

	return CC_SUCCESS;
}

ccReturn ccWindowSetMouseCursor(ccCursor cursor)
{
	XColor black;

	ccAssert(_ccWindow);

	if(XWINDATA->XCursor != 0){
		XFreeCursor(XWINDATA->XDisplay, XWINDATA->XCursor);
	}

	if(cursor != CC_CURSOR_NONE){
		XWINDATA->XCursor = XCreateFontCursor(XWINDATA->XDisplay, cursorList[cursor]);
		if(!XWINDATA->XCursor){
			ccErrorPush(CC_ERROR_WINDOW_CURSOR);
		}
	}else{
		black.red = black.green = black.blue = 0;
		XWINDATA->XCursor = XCreatePixmapCursor(XWINDATA->XDisplay, XWINDATA->XEmptyCursorImage, XWINDATA->XEmptyCursorImage, &black, &black, 0, 0);
	}

	XDefineCursor(XWINDATA->XDisplay, XWINDATA->XWindow, XWINDATA->XCursor);

	return CC_SUCCESS;
}

ccReturn ccWindowClipboardSetString(const char *text)
{
	Atom atom;

	ccAssert(_ccWindow);

	if(!text){
		text = "";
	}

	atom = XInternAtom(XWINDATA->XDisplay, "CLIPBOARD", False);
	XChangeProperty(XWINDATA->XDisplay, RootWindow(XWINDATA->XDisplay, XWINDATA->XScreen), XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace, (const unsigned char*)text, strlen(text));

	if(atom != None && XGetSelectionOwner(XWINDATA->XDisplay, atom) != XWINDATA->XWindow){
		XSetSelectionOwner(XWINDATA->XDisplay, atom, XWINDATA->XWindow, CurrentTime);
	}

	if(XGetSelectionOwner(XWINDATA->XDisplay, XA_PRIMARY) != XWINDATA->XWindow){
		XSetSelectionOwner(XWINDATA->XDisplay, XA_PRIMARY, XWINDATA->XWindow, CurrentTime);
	}

	return CC_SUCCESS;
}

char *ccWindowClipboardGetString()
{
	return NULL;
}

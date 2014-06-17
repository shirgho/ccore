#include "../../common/interface/window.h"

static ccDisplays *_displays;
static ccWindow *_window;

/* Attribute list for a double buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrList[] =                                             
{                                                                      
	GLX_RGBA, GLX_DOUBLEBUFFER,                                        
	GLX_RED_SIZE, 4,                                                   
	GLX_GREEN_SIZE, 4,                                                 
	GLX_BLUE_SIZE, 4,                                                  
	GLX_DEPTH_SIZE, 16,                                                
};

void ccNewWindow(ccRect rect, const char *title, int flags)
{
	Window root;
	Atom delete;

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
	glXMakeCurrent(_window->XDisplay, None, NULL);
	glXDestroyContext(_window->XDisplay, _window->XContext);
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
	}else if(mode == CC_WINDOW_MODE_MINIMIZED){
		XIconifyWindow(_window->XDisplay, _window->XWindow, _window->XScreen);

		XUngrabPointer(_window->XDisplay, CurrentTime);
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

ccError ccSetResolution(ccDisplay *display, int resolutionIndex)
{
	int minX, minY, maxX, maxY;
	ccDisplayData *displayData;
	Display *XDisplay;
	Window root;
	XRRScreenResources *resources;
	XRROutputInfo *outputInfo;
	XRRCrtcInfo *crtcInfo;

	/* Screen already has the good coordinates */
	if(resolutionIndex >= 0){
		displayData = display->resolution + resolutionIndex;
		if(display->resolution->width == displayData->width && display->resolution->height == displayData->height){
			return CC_ERROR_NONE;
		}

		if(display->resolution->width <= 0 || display->resolution->height <= 0){
			ccPrintString("Error: Resolution supplied not valid\n");
			return CC_ERROR_RESOLUTION_CHANGE;
		}
	}else{
		/* Supress warnings */
		displayData = NULL;
	}

	XDisplay = XOpenDisplay(display->XDisplayName);
	root = RootWindow(XDisplay, display->XScreen);
	XGrabServer(XDisplay);

	if(!XRRGetScreenSizeRange(XDisplay, root, &minX, &minY, &maxX, &maxY)){
		ccPrintString("X: Unable to get screen size range\n");
		return CC_ERROR_RESOLUTION_CHANGE;
	}

	if(resolutionIndex >= 0){
		if(displayData->width < minX || displayData->height < minY){
			ccPrintString("X: Unable to set size of screen below the minimum of %dx%d\n", minX, minY);
			return CC_ERROR_RESOLUTION_CHANGE;
		} else if(displayData->width > maxX || displayData->height > maxY){
			ccPrintString("X: Unable to set size of screen above the maximum of %dx%d\n", maxX, maxY);
			return CC_ERROR_RESOLUTION_CHANGE;
		}

		ccPrintString("X: Setting display %d to %dx%d\n", display->XScreen, displayData->width, displayData->height);
	}

	resources = XRRGetScreenResources(XDisplay, root);
	if(!resources){
		ccPrintString("X: Couldn't get screen resources");
		return CC_ERROR_RESOLUTION_CHANGE;
	}
	outputInfo = XRRGetOutputInfo(XDisplay, resources, display->XOutput);
	if(!outputInfo || outputInfo->connection == RR_Disconnected){
		ccPrintString("X: Couldn't get output info");
		XRRFreeScreenResources(resources);
		return CC_ERROR_RESOLUTION_CHANGE;
	}
	crtcInfo = XRRGetCrtcInfo(XDisplay, resources, outputInfo->crtc);
	if(!crtcInfo){
		ccPrintString("X: Couldn't get crtc info");
		XRRFreeScreenResources(resources);
		XRRFreeOutputInfo(outputInfo);
		return CC_ERROR_RESOLUTION_CHANGE;
	}

	if(resolutionIndex >= 0){
		XRRSetCrtcConfig(XDisplay, resources, outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, displayData->XMode, crtcInfo->rotation, &display->XOutput, 1);
	}else{
		XRRSetCrtcConfig(XDisplay, resources, outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, display->XOldMode, crtcInfo->rotation, &display->XOutput, 1);
	}

	XRRFreeScreenResources(resources);
	XRRFreeOutputInfo(outputInfo);
	XRRFreeCrtcInfo(crtcInfo);

	if(resolutionIndex >= 0){
		display->resolution = displayData;
	}

	XSync(XDisplay, False);
	XUngrabServer(XDisplay);
	XCloseDisplay(XDisplay);

	return CC_ERROR_NONE;
}

bool ccResolutionExists(ccDisplay *display, ccDisplayData *resolution)
{
	int i;

	for(i = 0; i < display->amount; i++){
		if(resolution->width == display->resolution[i].width && resolution->height == display->resolution[i].height){
			return true;
		}
	}

	return false;
}

static bool ccXFindDisplaysXinerama(Display *display, char *displayName)
{
	int i, j, k, displayNameLength, eventBase, errorBase;
	unsigned int vTotal;
	ccDisplay *currentDisplay;
	ccDisplayData currentResolution;
	Window root;
	XRRScreenResources *resources;
	XRROutputInfo *outputInfo;
	XRRCrtcInfo *crtcInfo;

	if(!XineramaQueryExtension(display, &eventBase, &errorBase) || !XineramaIsActive(display)){
		ccPrintString("Xinerama not supported or active\n");
		return false;
	}

	currentResolution.bitDepth = -1;

	root = RootWindow(display, 0);
	resources = XRRGetScreenResources(display, root);

	for(i = 0; i < resources->noutput; i++){
		outputInfo = XRRGetOutputInfo(display, resources, resources->outputs[i]);
		/* Ignore disconnected devices */
		if(outputInfo->connection != 0){
#ifdef DEBUG
			ccPrintString("X: Ignored disconnected display %d\n", i);
#endif
			continue;
		}

		_displays->amount++;
		if(_displays->amount == 1){
			_displays->display = malloc(sizeof(ccDisplay));
		}else{
			_displays->display = realloc(_displays->display, sizeof(ccDisplay) * _displays->amount);
		}
		currentDisplay = _displays->display + _displays->amount - 1;

		displayNameLength = strlen(displayName);
		currentDisplay->XDisplayName = malloc(displayNameLength + 1);
		memcpy(currentDisplay->XDisplayName, displayName, displayNameLength);

		currentDisplay->monitorName = malloc(outputInfo->nameLen + 1);
		memcpy(currentDisplay->monitorName, outputInfo->name, outputInfo->nameLen);

		currentDisplay->XDisplayName[displayNameLength] = '\0';
		currentDisplay->gpuName = "";

		crtcInfo = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
		if(crtcInfo){
			currentDisplay->x = crtcInfo->x;
			currentDisplay->y = crtcInfo->y;
			currentDisplay->XOldMode = crtcInfo->mode;
		}else{
			currentDisplay->x = -1;
			currentDisplay->y = -1;
		}

		currentDisplay->XineramaScreen = i;
		currentDisplay->XScreen = 0;
		currentDisplay->XOutput = resources->outputs[i];
		currentDisplay->current = 0;
		currentDisplay->amount = 0;

		for(j = 0; j < outputInfo->nmode; j++){
			for(k = 0; k < resources->nmode; k++){
				if(outputInfo->modes[j] == resources->modes[k].id){
					vTotal = resources->modes[k].vTotal;
					if(resources->modes[k].modeFlags & RR_DoubleScan){
						vTotal <<= 1;
					}
					if(resources->modes[k].modeFlags & RR_Interlace){
						vTotal >>= 1;
					}

					currentResolution.XMode = outputInfo->modes[j];
					currentResolution.refreshRate = resources->modes[k].dotClock / (resources->modes[k].hTotal * vTotal);
					currentResolution.width = resources->modes[k].width;
					currentResolution.height = resources->modes[k].height;

					currentDisplay->amount++;
					if(currentDisplay->amount == 1){
						currentDisplay->resolution = malloc(sizeof(ccDisplayData));
					}else{
						currentDisplay->resolution = realloc(currentDisplay->resolution, sizeof(ccDisplayData) * currentDisplay->amount);
					}
					memcpy(currentDisplay->resolution + (currentDisplay->amount - 1), &currentResolution, sizeof(ccDisplayData));
					break;
				}
			}
		}

		XRRFreeCrtcInfo(crtcInfo);
		XRRFreeOutputInfo(outputInfo);
	}

	XRRFreeScreenResources(resources);

	return true;
}

static void ccXFindDisplaysXrandr(Display *display, char *displayName)
{
	int i, j, k, screenCount, sizeCount, rateCount, displayNameLength;
	short *refreshRates;
	ccDisplay *currentDisplay;
	ccDisplayData currentResolution;
	Window root;
	Rotation rotation;
	SizeID currentSize;
	XWindowAttributes attrList;
	XRRScreenConfiguration *screenConfig;
	XRRScreenSize *sizes;

	screenCount = XScreenCount(display);

	ccPrintString("XRandr: Found %d displays\n", screenCount);

	for(i = 0; i < screenCount; i++){
		_displays->amount++;
		if(_displays->amount == 1){
			_displays->display = malloc(sizeof(ccDisplay));
		}else{
			_displays->display = realloc(_displays->display, sizeof(ccDisplay) * _displays->amount);
		}
		currentDisplay = _displays->display + _displays->amount - 1;

		displayNameLength = strlen(displayName);
		currentDisplay->monitorName = malloc(displayNameLength + 1);
		memcpy(currentDisplay->monitorName, displayName, displayNameLength);
		currentDisplay->monitorName[displayNameLength] = '\0';
		currentDisplay->gpuName = "";

		root = RootWindow(display, i);
		XGetWindowAttributes(display, root, &attrList);

		currentDisplay->XineramaScreen = -1;
		currentDisplay->XScreen = i;
		currentDisplay->x = attrList.x;
		currentDisplay->y = attrList.y;

		screenConfig = XRRGetScreenInfo(display, root);
		currentSize = XRRConfigCurrentConfiguration(screenConfig, &rotation);
		sizes = XRRConfigSizes(screenConfig, &sizeCount);

		for(j = 0; j < sizeCount; j++){
			currentResolution.width = sizes[j].width;
			currentResolution.height = sizes[j].height;

			if(j == currentSize){
				currentDisplay->current = j;
			}

			refreshRates = XRRRates(display, currentDisplay->XScreen, j, &rateCount);
			rateCount = 0;
			for(k = 0; k < rateCount; k++){
				currentResolution.refreshRate = refreshRates[k];

				currentResolution.bitDepth = attrList.depth;

				currentDisplay->amount++;
				if(currentDisplay->amount == 1){
					currentDisplay->resolution = malloc(sizeof(ccDisplayData));
				}else{
					currentDisplay->resolution = realloc(currentDisplay->resolution, sizeof(ccDisplayData) * currentDisplay->amount);
				}
				memcpy(currentDisplay->resolution + (currentDisplay->amount - 1), &currentResolution, sizeof(ccDisplayData));
			}
		}
	}
}

void ccFindDisplays()
{
	char displayName[64];
	DIR *dir;
	struct dirent *direntry;
	Display *display;

	if(_displays->amount != 0){
		ccPrintString("Displays already found, no need to call ccFindDisplays anymore\n");
		return;
	}

	_displays->amount = 0;

	dir = opendir("/tmp/.X11-unix");
	ccAssert(dir != NULL);

	while((direntry = readdir(dir)) != NULL){
		if(direntry->d_name[0] != 'X'){
			continue;
		}
		snprintf(displayName, 64, ":%s", direntry->d_name + 1);
		ccPrintString("X: Found root display %s\n", displayName);
		display = XOpenDisplay(displayName);
		if(display != NULL){
			if(!ccXFindDisplaysXinerama(display, displayName)){
				ccXFindDisplaysXrandr(display, displayName);
			}		
			ccPrintString("X: %d displays found\n", _displays->amount);
			XCloseDisplay(display);
		}
	}
}

void ccUpdateDisplays()
{

}

void ccFreeDisplays()
{
	/*	int i;

		for(i = 0; i < _displays->amount; i++){
		if(_displays->display + i != NULL){
		free(_displays->display[i].gpuName);
		free(_displays->display[i].monitorName);
		free(_displays->display[i].resolution);
		}
		} */
	free(_displays->display);
}

void ccRevertDisplays()
{
	int i;

	for(i = 0; i < _displays->amount; i++){
		ccSetResolution(_displays->display + i, -1);
	}
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

int ccGetDisplayAmount()
{
	return _displays->amount;
}

ccDisplay *ccGetDisplay(int index)
{
	return _displays->display + index;
}

ccDisplay *ccGetDefaultDisplay()
{
	return _displays->display + _displays->primary;
}

ccError ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	ccAssert(_window != NULL);

	visual = glXChooseVisual(_window->XDisplay, _window->XScreen, attrList);
	ccAssert(visual != NULL);

	_window->XContext = glXCreateContext(_window->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(_window->XDisplay, _window->XWindow, _window->XContext);

	if(glewInit() != GLEW_OK){
		return CC_ERROR_GLEWINIT;
	}

	return CC_ERROR_NONE;
}

void ccGLSwapBuffers()
{
	glXSwapBuffers(_window->XDisplay, _window->XWindow);
}

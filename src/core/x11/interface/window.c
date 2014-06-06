#include "../../common/interface/window.h"

static ccDisplays _displays;

/* Attribute list for a double buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrList[] =                                             
{                                                                      
	GLX_RGBA, GLX_DOUBLEBUFFER,                                        
	GLX_RED_SIZE, 4,                                                   
	GLX_GREEN_SIZE, 4,                                                 
	GLX_BLUE_SIZE, 4,                                                  
	GLX_DEPTH_SIZE, 16,                                                
};

ccWindow* ccNewWindow(ccRect rect, const char *title, int flags)
{
	ccWindow *output;
	Window root;
	Atom delete;

	output = malloc(sizeof(ccWindow));
	output->XDisplay = XOpenDisplay(NULL);
	//TODO: change assert to error
	assert(output->XDisplay != NULL);
	root = DefaultRootWindow(output->XDisplay);
	output->XScreen = DefaultScreen(output->XDisplay);
	output->XWindow = XCreateSimpleWindow(output->XDisplay, root, rect.x, rect.y, rect.width, rect.height, 1, BlackPixel(output->XDisplay, output->XScreen), WhitePixel(output->XDisplay, output->XScreen));
	// Choose types of events
	XSelectInput(output->XDisplay, output->XWindow, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask);
	XMapWindow(output->XDisplay, output->XWindow);
	XStoreName(output->XDisplay, output->XWindow, title);

	output->mouse.x = output->mouse.y = output->rect.width = output->rect.height = -1;

	delete = XInternAtom(output->XDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(output->XDisplay, output->XWindow, &delete, 1);

	return output;
}

void ccFreeWindow(ccWindow *window)
{
	assert(window != NULL);
	//TODO: don't delete a context before checking whether it exists!
	glXMakeCurrent(window->XDisplay, None, NULL);
	glXDestroyContext(window->XDisplay, window->XContext);
	XCloseDisplay(window->XDisplay);
	free(window);
}

bool ccPollEvent(ccWindow *window)
{
	XEvent event;
	XWindowAttributes windowAttributes;

	assert(window != NULL);

	window->event.type = CC_EVENT_SKIP;
	if(XPending(window->XDisplay) == 0){
		return false;
	}

	XNextEvent(window->XDisplay, &event);
	switch(event.type){
		case ButtonPress:
			// 1 = left, 2 = middle, 3 = right, 4 = scroll up, 5 = scroll down
			if(event.xbutton.button <= 3){
				window->event.type = CC_EVENT_MOUSE_DOWN;
				window->event.mouseButton = event.xbutton.button;
			}else if(event.xbutton.button == 4){
				window->event.type = CC_EVENT_MOUSE_SCROLL;
				window->event.scrollDelta = 1;
			}else if(event.xbutton.button == 5){
				window->event.type = CC_EVENT_MOUSE_SCROLL;
				window->event.scrollDelta = -1;
			}
			break;
		case ButtonRelease:
			window->event.type = CC_EVENT_MOUSE_UP;
			window->event.mouseButton = event.xbutton.button;
			break;
		case MotionNotify:
			if(window->mouse.x != event.xmotion.x || window->mouse.y != event.xmotion.y){
				window->event.type = CC_EVENT_MOUSE_MOVE;
				window->mouse.x = event.xmotion.x;
				window->mouse.y = event.xmotion.y;
			}
			break;
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;
		case KeyPress:
			window->event.type = CC_EVENT_KEY_DOWN;
			window->event.key = ccXLookupKey(XLookupKeysym(&event.xkey, 0));
			break;
		case KeyRelease:
			window->event.type = CC_EVENT_KEY_UP;
			window->event.key = ccXLookupKey(XLookupKeysym(&event.xkey, 0));
			break;
		case ConfigureNotify:
			if(window->rect.width != event.xconfigure.width || window->rect.height != event.xconfigure.height){
				window->event.type = CC_EVENT_WINDOW_RESIZE;
				window->rect.width = event.xconfigure.width;
				window->rect.height = event.xconfigure.height;
				window->aspect = window->rect.height / window->rect.width;
				XGetWindowAttributes(window->XDisplay, window->XWindow, &windowAttributes);
				window->rect.x = windowAttributes.x;
				window->rect.y = windowAttributes.y;
			}
			break;
		case ClientMessage:
			window->event.type = CC_EVENT_WINDOW_QUIT;
			break;
		case EnterNotify:
			window->event.type = CC_EVENT_FOCUS_GAINED;
			break;
		case LeaveNotify:
			window->event.type = CC_EVENT_FOCUS_LOST;
			break;
		case FocusIn:
			window->event.type = CC_EVENT_FOCUS_GAINED;
			break;
		case FocusOut:
			window->event.type = CC_EVENT_FOCUS_LOST;
			break;
	}

	return true;
}

void ccChangeWM(ccWindow *window, ccWindowMode mode)
{
	XEvent event;
	XWindowAttributes windowAttributes;
	Atom wmState, fullscreen;
	int windowWidth, windowHeight;

	if(mode == CC_WINDOW_MODE_FULLSCREEN || mode == CC_WINDOW_MODE_WINDOW){
		wmState = XInternAtom(window->XDisplay, "_NET_WM_STATE", false);
		fullscreen = XInternAtom(window->XDisplay, "_NET_WM_STATE_FULLSCREEN", false);

		memset(&event, 0, sizeof(event));
		event.type = ClientMessage;
		event.xclient.window = window->XWindow;
		event.xclient.message_type = wmState;
		event.xclient.format = 32;
		event.xclient.data.l[0] = mode == CC_WINDOW_MODE_FULLSCREEN;
		event.xclient.data.l[1] = fullscreen;

		XSendEvent(window->XDisplay, DefaultRootWindow(window->XDisplay), false, SubstructureNotifyMask, &event);
	}else if(mode == CC_WINDOW_MODE_MINIMIZED){
		XUnmapWindow(window->XDisplay, window->XWindow);
	}else if(mode == CC_WINDOW_MODE_MAXIMIZED){
		XGetWindowAttributes(window->XDisplay, DefaultRootWindow(window->XDisplay), &windowAttributes);
		windowWidth = windowAttributes.width - windowAttributes.x - (windowAttributes.border_width << 1);
		windowHeight = windowAttributes.height - windowAttributes.y - (windowAttributes.border_width << 1);
		XMoveResizeWindow(window->XDisplay, window->XWindow, 0, 0, windowWidth, windowHeight);
	}
}

void ccResizeMoveWindow(ccWindow *window, ccRect rect)
{
	XMoveResizeWindow(window->XDisplay, window->XWindow, rect.x, rect.y, rect.width, rect.height);
}

void ccCenterWindow(ccWindow *window)
{

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

bool ccFindDisplaysXinerama(Display *display, char *displayName)
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
#ifdef DEBUG
		printf("Xinerama not supported or active\n");
#endif
		return false;
	}

	currentResolution.bitDepth = -1;

	root = RootWindow(display, 0);
	resources = XRRGetScreenResources(display, root);

	for(i = 0; i < resources->noutput; i++){
		outputInfo = XRRGetOutputInfo(display, resources, resources->outputs[i]);
		/* Ignore disconnected devices */
		if(outputInfo->connection != 0){
			continue;
		}

		_displays.amount++;
		if(_displays.amount == 1){
			_displays.display = malloc(sizeof(ccDisplay));
		}else{
			_displays.display = realloc(_displays.display, sizeof(ccDisplay) * _displays.amount);
		}
		currentDisplay = _displays.display + _displays.amount - 1;

		displayNameLength = strlen(displayName);
		currentDisplay->monitorName = malloc(displayNameLength + 1);
		memcpy(currentDisplay->monitorName, displayName, displayNameLength);

		currentDisplay->monitorName[displayNameLength] = '\0';
		currentDisplay->gpuName = "";

		// Dangerous operation
		crtcInfo = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);

		currentDisplay->x = crtcInfo->x;
		currentDisplay->y = crtcInfo->y;
		currentDisplay->XineramaScreen = i;
		currentDisplay->XScreen = 0;
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

void ccFindDisplaysXrandr(Display *display, char *displayName)
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

#ifdef DEBUG
	printf("XRandr: Found %d displays\n", screenCount);
#endif

	for(i = 0; i < screenCount; i++){
		_displays.amount++;
		if(_displays.amount == 1){
			_displays.display = malloc(sizeof(ccDisplay));
		}else{
			_displays.display = realloc(_displays.display, sizeof(ccDisplay) * _displays.amount);
		}
		currentDisplay = _displays.display + _displays.amount - 1;

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

	if(_displays.amount != 0){
#ifdef DEBUG
		printf("Displays already found, no need to call ccFindDisplays anymore\n");
#endif
		return;
	}

	_displays.amount = 0;

	dir = opendir("/tmp/.X11-unix");
	assert(dir != NULL);

	while((direntry = readdir(dir)) != NULL){
		if(direntry->d_name[0] != 'X'){
			continue;
		}
		snprintf(displayName, 64, ":%s", direntry->d_name + 1);
#ifdef DEBUG
		printf("X: Found display %s\n", displayName);
#endif	
		display = XOpenDisplay(displayName);
		if(display != NULL){
			if(!ccFindDisplaysXinerama(display, displayName)){
				ccFindDisplaysXrandr(display, displayName);
			}		
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

	for(i = 0; i < _displays.amount; i++){
		if(_displays.display + i != NULL){
			free(_displays.display[i].gpuName);
			free(_displays.display[i].monitorName);
			free(_displays.display[i].resolution);
		}
	} */
	free(_displays.display);
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
	return _displays.amount;
}

ccDisplay *ccGetDisplay(int index)
{
	return _displays.display + index;
}

ccDisplay *ccGetDefaultDisplay()
{
	return _displays.display + _displays.primary;
}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	assert(window != NULL);

	visual = glXChooseVisual(window->XDisplay, window->XScreen, attrList);
	assert(visual != NULL);

	window->XContext = glXCreateContext(window->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(window->XDisplay, window->XWindow, window->XContext);
}

void ccGLSwapBuffers(ccWindow *window)
{
	glXSwapBuffers(window->XDisplay, window->XWindow);
}

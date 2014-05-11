#include "../../core/interface/window.h"

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
	ccAssert(output->XDisplay != NULL);
	root = DefaultRootWindow(output->XDisplay);
	output->XScreen = DefaultScreen(output->XDisplay);
	output->XWindow = XCreateSimpleWindow(output->XDisplay, root, 10, 10, rect.width, rect.height, 1, BlackPixel(output->XDisplay, output->XScreen), WhitePixel(output->XDisplay, output->XScreen));
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
	ccAssert(window != NULL);
	//TODO: don't delete a context before checking whether it exists!
	glXMakeCurrent(window->XDisplay, None, NULL);
	glXDestroyContext(window->XDisplay, window->XContext);
	XCloseDisplay(window->XDisplay);
	free(window);
}

bool ccPollEvent(ccWindow *window)
{
	XEvent event;

	ccAssert(window != NULL);

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
	Atom wmState, fullscreen;

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
	}
}

void ccResizeWindow(ccWindow *window, ccRect rect)
{

}

void ccCenterWindow(ccWindow *window)
{

}

ccResolutions *ccGetResolutions(ccDisplay *display)
{
	ccResolutions *output;

	output = malloc(sizeof(ccResolutions));

	return output;
}

bool ccResolutionExists(ccDisplay *display, ccDisplayData *resolution)
{

	return false;
}

void ccFindDisplays()
{
	DIR *dir;
	struct dirent *direntry;
	Display *disp;
	ccDisplay current;
	XWindowAttributes attrList;
	char displayName[64];
	int i, screenCount;

	displays.amount = 0;

	dir = opendir("/tmp/.X11-unix");
	ccAssert(dir != NULL);

	while((direntry = readdir(dir)) != NULL){
		if(direntry->d_name[0] != 'X'){
			continue;
		}
		memset(displayName, '\0', 64);
		strcat(displayName, direntry->d_name);
		displayName[0] = ':';
		disp = XOpenDisplay(displayName);
		if(disp != NULL){
			screenCount = XScreenCount(disp);
			for(i = 0; i < screenCount; i++){
				XGetWindowAttributes(disp, RootWindow(disp, i), &attrList);

				memcpy(current.monitorName, displayName, 64);
				current.gpuName[0] = '\0';
				current.XScreen = i;
				current.x = attrList.x;
				current.y = attrList.y;
				current.displayData.width = attrList.width;
				current.displayData.height = attrList.height;
				current.displayData.bitDepth = attrList.depth;

				displays.amount++;
				if(displays.amount == 1){
					displays.display = malloc(sizeof(ccDisplay));
				}else{
					displays.display = realloc(displays.display, sizeof(ccDisplay) * displays.amount);
				}
				memcpy(displays.display + (displays.amount - 1), &current, sizeof(ccDisplay));
			}

			XCloseDisplay(disp);
		}
	}
}

void ccUpdateDisplays()
{

}

ccDisplays *ccGetDisplays()
{
	return &displays;
}

ccDisplay *ccGetDefaultDisplay()
{
	ccDisplay *output;

	output = malloc(sizeof(ccDisplay));

	return output;
}

void ccFreeDisplays()
{
	free(displays.display);
}

void ccGetDisplayRect(ccDisplay *display, ccRect *rect)
{

}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	ccAssert(window != NULL);

	visual = glXChooseVisual(window->XDisplay, window->XScreen, attrList);
	ccAssert(visual != NULL);

	window->XContext = glXCreateContext(window->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(window->XDisplay, window->XWindow, window->XContext);
}

void ccGLSwapBuffers(ccWindow *window)
{
	glXSwapBuffers(window->XDisplay, window->XWindow);
}

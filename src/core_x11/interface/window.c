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

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title, ccWindowMode mode, int flags)
{
	ccWindow *output;
	Window root;

	output = malloc(sizeof(ccWindow));
	output->display = XOpenDisplay(NULL);
	//TODO: change assert to error
	ccAssert(output->display != NULL);
	root = DefaultRootWindow(output->display);
	output->screen = DefaultScreen(output->display);
	output->window = XCreateSimpleWindow(output->display, root, 10, 10, width, height, 1, BlackPixel(output->display, output->screen), WhitePixel(output->display, output->screen));
	// Choose types of events
	XSelectInput(output->display, output->window, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask);
	XMapWindow(output->display, output->window);
	XStoreName(output->display, output->window, title);

	if(mode != ccWMWindow){
		ccChangeWM(output, mode);
	}
	
	return output;
}

void ccFreeWindow(ccWindow *window)
{
	ccAssert(window != NULL);
	//TODO: don't delete a context before checking whether it exists!
	glXMakeCurrent(window->display, None, NULL);
	glXDestroyContext(window->display, window->context);
	XCloseDisplay(window->display);
	free(window);
}

bool ccPollEvent(ccWindow *window)
{
	XEvent event;

	ccAssert(window != NULL);

	// Remove this statemnt if you want to block until an event happens
	if(XPending(window->display) == 0){
		window->event.type = ccEventSkip;
		return false;
	}

	XNextEvent(window->display, &event);
	switch(event.type){
		case ButtonPress:
			window->event.type = ccEventMouseDown;
			window->event.mouseState.location = (ccPoint){event.xmotion.x, event.xmotion.y};
			break;
		case ButtonRelease:
			window->event.type = ccEventMouseUp;
			window->event.mouseState.location = (ccPoint){event.xmotion.x, event.xmotion.y};
			break;
		case MotionNotify:
			window->event.type = ccEventMouseMove;
			window->event.mouseState.location = (ccPoint){event.xmotion.x, event.xmotion.y};
			break;
		case KeyPress:
			window->event.type = ccEventKeyDown;
			break;
		case ConfigureNotify:
			window->event.type = ccEventWindowResize;
			window->width = event.xconfigure.width;
			window->height = event.xconfigure.height;
			break;
		case EnterNotify:
			window->event.type = ccEventMouseFocusGained;
			break;
		case LeaveNotify:
			window->event.type = ccEventMouseFocusLost;
			break;
		case FocusIn:
			window->event.type = ccEventKeyboardFocusGained;
			break;
		case FocusOut:
			window->event.type = ccEventKeyboardFocusLost;
			break;
	}

	return true;
}

void ccChangeWM(ccWindow *window, ccWindowMode mode)
{
	XEvent event;
	Atom wmState, fullscreen;

	if(mode == ccWMFullScreen || mode == ccWMWindow){
		wmState = XInternAtom(window->display, "_NET_WM_STATE", false);
		fullscreen = XInternAtom(window->display, "_NET_WM_STATE_FULLSCREEN", false);

		memset(&event, 0, sizeof(event));
		event.type = ClientMessage;
		event.xclient.window = window->window;
		event.xclient.message_type = wmState;
		event.xclient.format = 32;
		event.xclient.data.l[0] = mode == ccWMFullScreen;
		event.xclient.data.l[1] = fullscreen;

		XSendEvent(window->display, DefaultRootWindow(window->display), false, SubstructureNotifyMask, &event);
	}
}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	ccAssert(window != NULL);

	visual = glXChooseVisual(window->display, window->screen, attrList);
	ccAssert(visual != NULL);

	window->context = glXCreateContext(window->display, visual, NULL, GL_TRUE);
	glXMakeCurrent(window->display, window->window, window->context);
}

void ccGLSwapBuffers(ccWindow *window)
{
	glXSwapBuffers(window->display, window->window);
}


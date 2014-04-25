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
	XSelectInput(output->display, output->window, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask | KeyPressMask | KeyReleaseMask);
	XMapWindow(output->display, output->window);
	XStoreName(output->display, output->window, title);

	if(mode != CC_WINDOW_MODE_WINDOW){
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
		window->event.type = CC_EVENT_SKIP;
		return false;
	}

	XNextEvent(window->display, &event);
	switch(event.type){
		case ButtonPress:
			// 1 = left, 2 = middle, 3 = right, 4 = scroll up, 5 = scroll down
			if(event.xbutton.button <= 3){
				window->event.type = CC_EVENT_MOUSE_DOWN;
				window->event.mouseState.button = event.xbutton.button;
			}else if(event.xbutton.button == 4){
				window->event.type = CC_EVENT_MOUSE_SCROLL_UP;
			}else if(event.xbutton.button == 5){
				window->event.type = CC_EVENT_MOUSE_SCROLL_DOWN;
			}
			break;
		case ButtonRelease:
			window->event.type = CC_EVENT_MOUSE_UP;
			window->event.mouseState.button = event.xbutton.button;
			break;
		case MotionNotify:
			window->event.type = CC_EVENT_MOUSE_MOVE;
			window->mouseX = event.xmotion.x;
			window->mouseY = event.xmotion.y;
			break;
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;
		case KeyPress:
			window->event.type = CC_EVENT_KEY_DOWN;
			break;
		case ConfigureNotify:
			window->event.type = CC_EVENT_WINDOW_RESIZE;
			window->width = event.xconfigure.width;
			window->height = event.xconfigure.height;
			window->aspect = window->height / window->width;
			break;
		case EnterNotify:
			window->event.type = CC_EVENT_MOUSE_FOCUS_GAINED;
			break;
		case LeaveNotify:
			window->event.type = CC_EVENT_MOUSE_FOCUS_LOST;
			break;
		case FocusIn:
			window->event.type = CC_EVENT_KEYBOARD_FOCUS_GAINED;
			break;
		case FocusOut:
			window->event.type = CC_EVENT_KEYBOARD_FOCUS_LOST;
			break;
	}

	return true;
}

void ccChangeWM(ccWindow *window, ccWindowMode mode)
{
	XEvent event;
	Atom wmState, fullscreen;

	if(mode == CC_WINDOW_MODE_FULLSCREEN || mode == CC_WINDOW_MODE_WINDOW){
		wmState = XInternAtom(window->display, "_NET_WM_STATE", false);
		fullscreen = XInternAtom(window->display, "_NET_WM_STATE_FULLSCREEN", false);

		memset(&event, 0, sizeof(event));
		event.type = ClientMessage;
		event.xclient.window = window->window;
		event.xclient.message_type = wmState;
		event.xclient.format = 32;
		event.xclient.data.l[0] = mode == CC_WINDOW_MODE_FULLSCREEN;
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


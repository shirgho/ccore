#include "../../core/interface/window.h"

/* Attribute list for a single buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrListSingleBuffered[] =
{
    GLX_RGBA, GLX_RED_SIZE, 4,                                         
    GLX_GREEN_SIZE, 4,                                                 
    GLX_BLUE_SIZE, 4,                                                  
    GLX_DEPTH_SIZE, 16,                                                
    None
};
/* Attribute list for a double buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrListDoubleBuffered[] =                                             
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
	if(output->display == NULL){
		return NULL;
	}
	root = DefaultRootWindow(output->display);
	output->screen = DefaultScreen(output->display);
	output->window = XCreateSimpleWindow(output->display, root, 10, 10, width, height, 1, BlackPixel(output->display, output->screen), WhitePixel(output->display, output->screen));
	/* Choose types of events */
	XSelectInput(output->display, output->window, ExposureMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask);
	XMapWindow(output->display, output->window);
	XStoreName(output->display, output->window, title);
	
	return output;
}

void ccFreeWindow(ccWindow *window)
{
	//TODO: don't delete a context before checking whether it exists!
	glXMakeCurrent(window->display, None, NULL);
	glXDestroyContext(window->display, window->context);
	XCloseDisplay(window->display);
	free(window);
}

bool ccPollEvent(ccWindow *window)
{
	XEvent event;

	/* Remove this statemnt if you want to block until an event happens */
	if(XPending(window->display) == 0){
		window->event.type = ccEventSkip;
		return false;
	}

	XNextEvent(window->display, &event);
	switch(event.type){
		case ButtonPress:
			window->event.type = ccEventMouseDown;
			window->event.mouseLocation = (ccPoint){event.xmotion.x, event.xmotion.y};
			break;
		case MotionNotify:
			window->event.type = ccEventMouseMove;
			window->event.mouseLocation = (ccPoint){event.xmotion.x, event.xmotion.y};
			break;
		case KeyPress:
			window->event.type = ccEventKeyDown;
			break;
		case ConfigureNotify:
			window->width = event.xconfigure.width;
			window->height = event.xconfigure.height;
			window->event.type = ccEventWindowResize;
			break;
	}

	return true;
}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;
	/*
	int major, minor;

	major = minor = 0;
	glXQueryVersion(window->display, &major, &minor);
	if(*glVersionMajor < major || (*glVersionMajor == major && *glVersionMinor < minor)){
		*glVersionMajor = major;
		*glVersionMinor = minor;
		return false;
	}
	*/

	visual = glXChooseVisual(window->display, window->screen, attrListDoubleBuffered);
	if(visual == NULL){
		window->isDoubleBuffered = false;
		visual = glXChooseVisual(window->display, window->screen, attrListSingleBuffered);
	}else{
		window->isDoubleBuffered = true;
	}

	window->context = glXCreateContext(window->display, visual, NULL, GL_TRUE);
	glXMakeCurrent(window->display, window->window, window->context);
	
	/*return true;*/
}

void ccGLSwapBuffers(ccWindow *window)
{
	glXSwapBuffers(window->display, window->window);
}


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

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char *title)
{
	ccWindow *output;
	Window root;

	output = malloc(sizeof(ccWindow));
	output->display = XOpenDisplay(NULL);
	if(output->display == NULL){
		return NULL;
	}
	/* TODO handle NULL */
	root = DefaultRootWindow(output->display);
	output->screen = DefaultScreen(output->display);
	output->window = XCreateSimpleWindow(output->display, root, 10, 10, width, height, 1, BlackPixel(output->display, output->screen), WhitePixel(output->display, output->screen));
	XSelectInput(output->display, output->window, ExposureMask | ButtonPressMask | StructureNotifyMask);
	XMapWindow(output->display, output->window);
	
	return output;
}

void ccFreeWindow(ccWindow *window)
{
	glXDestroyContext(window->display, window->context);
	XCloseDisplay(window->display);
	free(window);
}

bool ccPollEventWindow(ccWindow *window)
{
	XEvent event;

	XNextEvent(window->display, &event);
	switch(event.type){
		case ButtonPress:
			window->event.type = ccEventMouseDown;
			return true;
		case ConfigureNotify:
			window->width = event.xconfigure.width;
			window->height = event.xconfigure.height;
			window->event.type = ccEventWindowResize;
			return true;
	}

	return false;
}

bool ccGLBindContextWindow(ccWindow *window)
{
	XVisualInfo *visual;

	visual = glXChooseVisual(window->display, window->screen, attrListDoubleBuffered);
	if(visual == NULL){
		output->isDoubleBuffered = false;
		visual = glXChooseVisual(window->display, window->screen, attrListSingleBuffered);
	}else{
		output->isDoubleBuffered = true;
	}

	window->context = glXCreateContext(window->display, visual, NULL, GL_TRUE);
	glXMakeCurrent(window->display, window->window, window->context);
	
	return true;
}

void ccGLSwapBuffers(ccWindow *window)
{
	glXSwapBuffers(window->display, window->window);
}


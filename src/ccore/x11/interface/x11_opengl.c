#include "x11_opengl.h"

/* Attribute list for a double buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrList[] =
{
	GLX_RGBA, GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
};

ccError ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	ccAssert(_window != NULL);

	visual = glXChooseVisual(WINDOW_DATA->XDisplay, WINDOW_DATA->XScreen, attrList);
	if(!visual){
		return CC_ERROR_GLCONTEXT;
	}

	WINDOW_DATA->XContext = glXCreateContext(WINDOW_DATA->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, WINDOW_DATA->XContext);

	if(glewInit() != GLEW_OK){
		return CC_ERROR_GLEWINIT;
	}

	return CC_ERROR_NONE;
}

ccError ccGLFreeContext()
{
	if(!WINDOW_DATA->XContext){
		return CC_ERROR_GLCONTEXT;
	}
	glXDestroyContext(WINDOW_DATA->XDisplay, WINDOW_DATA->XContext);
	
	return CC_ERROR_NONE;
}

ccError ccGLSwapBuffers()
{
	if(!WINDOW_DATA->XContext){
		return CC_ERROR_GLCONTEXT;
	}
	glXSwapBuffers(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);

	return CC_ERROR_NONE;
}

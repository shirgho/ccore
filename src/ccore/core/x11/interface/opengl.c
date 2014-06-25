#include "../../common/interface/opengl.h"

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

	visual = glXChooseVisual(_window->XDisplay, _window->XScreen, attrList);
	ccAssert(visual != NULL);

	_window->XContext = glXCreateContext(_window->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(_window->XDisplay, _window->XWindow, _window->XContext);

	if(glewInit() != GLEW_OK){
		return CC_ERROR_GLEWINIT;
	}

	return CC_ERROR_NONE;
}

void ccGLFreeContext()
{
	glXDestroyContext(_window->XDisplay, _window->XContext);
}

void ccGLSwapBuffers()
{
	glXSwapBuffers(_window->XDisplay, _window->XWindow);
}
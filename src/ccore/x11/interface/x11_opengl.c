#include "x11_opengl.h"

/* Attribute list for a double buffered OpenGL context, with at least 4 bits per color and a 16 bit depth buffer */
static int attrList[] =
{
	GLX_RGBA, GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

ccReturn ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	XVisualInfo *visual;

	if(_ccWindow == NULL){
		ccErrorPush(CC_ERROR_WINDOW_NONE);
		return CC_FAIL;
	}

	visual = glXChooseVisual(WINDOW_DATA->XDisplay, WINDOW_DATA->XScreen, attrList);
	if(!visual){
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	WINDOW_DATA->XContext = glXCreateContext(WINDOW_DATA->XDisplay, visual, NULL, GL_TRUE);
	glXMakeCurrent(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow, WINDOW_DATA->XContext);

	if(glewInit() != GLEW_OK){
		ccErrorPush(CC_ERROR_GL_GLEWINIT);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccGLFreeContext(void)
{
	if(WINDOW_DATA->XContext == NULL){
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	glXDestroyContext(WINDOW_DATA->XDisplay, WINDOW_DATA->XContext);
	
	return CC_SUCCESS;
}

ccReturn ccGLSwapBuffers(void)
{
	if(WINDOW_DATA->XContext == NULL){
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	glXSwapBuffers(WINDOW_DATA->XDisplay, WINDOW_DATA->XWindow);

	return CC_SUCCESS;
}

bool ccGLHasContext(void)
{
	return WINDOW_DATA->XContext != NULL;
}

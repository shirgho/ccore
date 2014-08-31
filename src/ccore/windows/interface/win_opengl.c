#include "win_opengl.h"

ccError ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	int glVerMajor, glVerMinor;

	ccAssert(ccWindowExists());

	WINDOW_DATA->hdc = GetDC(WINDOW_DATA->winHandle);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,
		0, 0, 0, 0, 0, 0, 0
	};

	pixelFormatIndex = ChoosePixelFormat(WINDOW_DATA->hdc, &pfd);
	SetPixelFormat(WINDOW_DATA->hdc, pixelFormatIndex, &pfd);

	WINDOW_DATA->renderContext = wglCreateContext(WINDOW_DATA->hdc);
	if(WINDOW_DATA->renderContext == NULL) return CC_ERROR_GLCONTEXT;

	//Make window the current context
	wglMakeCurrent(WINDOW_DATA->hdc, WINDOW_DATA->renderContext);

	//Version check
	glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
	if(glVerMajor < glVersionMajor || (glVerMajor == glVersionMajor && glVerMinor < glVersionMinor)) return CC_ERROR_GLVERSION;

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) return CC_ERROR_GLEWINIT;

	return CC_ERROR_NONE;
}

ccError ccGLFreeContext(void)
{
	ccAssert(_ccWindow != NULL);

	wglDeleteContext(WINDOW_DATA->renderContext);

	return CC_ERROR_NONE;
}

ccError ccGLSwapBuffers(void)
{
	ccAssert(_ccWindow != NULL);
	return SwapBuffers(WINDOW_DATA->hdc) == TRUE?CC_ERROR_NONE:CC_ERROR_CANTSWAP;
}
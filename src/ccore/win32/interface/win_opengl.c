#include "win_opengl.h"

ccError ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	int glVerMajor, glVerMinor;

	ccAssert(ccWindowExists());

	_window->hdc = GetDC(_window->winHandle);

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

	pixelFormatIndex = ChoosePixelFormat(_window->hdc, &pfd);
	SetPixelFormat(_window->hdc, pixelFormatIndex, &pfd);

	_window->renderContext = wglCreateContext(_window->hdc);
	if(_window->renderContext == NULL) return CC_ERROR_GLCONTEXT;

	//Make window the current context
	wglMakeCurrent(_window->hdc, _window->renderContext);

	//Version check
	glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
	if(glVerMajor < glVersionMajor || (glVerMajor == glVersionMajor && glVerMinor < glVersionMinor)) return CC_ERROR_GLVERSION;

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) return CC_ERROR_GLEWINIT;

	return CC_ERROR_NONE;
}

ccError ccGLFreeContext()
{
	ccAssert(_window != NULL);

	wglDeleteContext(_window->renderContext);

	return CC_ERROR_NONE;
}

ccError ccGLSwapBuffers()
{
	ccAssert(_window != NULL);

	return SwapBuffers(_window->hdc) == TRUE?CC_ERROR_NONE:CC_ERROR_CANTSWAP;
}
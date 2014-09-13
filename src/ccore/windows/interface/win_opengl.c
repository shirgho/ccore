#include "win_opengl.h"

ccReturn ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	int glVerMajor, glVerMinor;

	ccAssert(ccWindowExists());

	WINDOW_DATA->hdc = GetDC(WINDOW_DATA->winHandle);
	if(WINDOW_DATA->hdc == NULL) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

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
	if(pixelFormatIndex == 0) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	SetPixelFormat(WINDOW_DATA->hdc, pixelFormatIndex, &pfd);

	WINDOW_DATA->renderContext = wglCreateContext(WINDOW_DATA->hdc);
	if(WINDOW_DATA->renderContext == NULL) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	//Make window the current context
	wglMakeCurrent(WINDOW_DATA->hdc, WINDOW_DATA->renderContext);

	//Version check
	glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
	if(glVerMajor < glVersionMajor || (glVerMajor == glVersionMajor && glVerMinor < glVersionMinor)) {
		ccErrorPush(CC_ERROR_GL_VERSION);
		return CC_FAIL;
	}

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) {
		ccErrorPush(CC_ERROR_GL_GLEWINIT);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccGLFreeContext(void)
{
	ccAssert(_ccWindow != NULL);

	wglDeleteContext(WINDOW_DATA->renderContext);
	WINDOW_DATA->renderContext = NULL;

	return CC_SUCCESS;
}

ccReturn ccGLSwapBuffers(void)
{
	ccAssert(_ccWindow != NULL);
	if(SwapBuffers(WINDOW_DATA->hdc) == TRUE) {
		return CC_SUCCESS;
	}
	else{
		ccErrorPush(CC_ERROR_GL_BUFFERSWAP);
		return CC_FAIL;
	}
}

bool ccGLHasContext(void)
{
	return WINDOW_DATA->renderContext != NULL;
}
#include "win_opengl.h"

ccReturn ccGLContextBind(int glVersionMajor, int glVersionMinor)
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

	if(SetPixelFormat(WINDOW_DATA->hdc, pixelFormatIndex, &pfd) == FALSE) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	WINDOW_DATA->renderContext = wglCreateContext(WINDOW_DATA->hdc);
	if(WINDOW_DATA->renderContext == NULL) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

	//Make window the current context
	if(wglMakeCurrent(WINDOW_DATA->hdc, WINDOW_DATA->renderContext) == FALSE) {
		ccErrorPush(CC_ERROR_GL_CONTEXT);
		return CC_FAIL;
	}

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

ccReturn ccGLContextFree(void)
{
	ccAssert(_ccWindow != NULL);

	wglDeleteContext(WINDOW_DATA->renderContext);
	WINDOW_DATA->renderContext = NULL;

	return CC_SUCCESS;
}

ccReturn ccGLBuffersSwap(void)
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

bool ccGLContextIsActive(void)
{
	return WINDOW_DATA->renderContext != NULL;
}
#pragma once

#include <string.h>
#include "../interface/popup.h"

typedef enum {
	// No errors
	CC_ERROR_NO = 0,
	// The window couldn't find a display to attach to
	CC_ERROR_NODisplay,
	// The target OpenGL version is not supported
	CC_ERROR_GL_UNSUPPORTED,
} ccError;

void ccErrorString(char *dest, ccError error);
void ccAbort(const char* message);

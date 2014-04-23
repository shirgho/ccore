#pragma once

#include <string.h>
#include "../interface/popup.h"

typedef enum {
	// No errors
	ccErrorNo = 0,
	// The window couldn't find a display to attach to
	ccErrorNoDisplay,
	// The target OpenGL version is not supported
	ccErrorGLVersionUnsupported,
} ccError;

void ccErrorString(char *dest, ccError error);
void ccAbort(const char* message);

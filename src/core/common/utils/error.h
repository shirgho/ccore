#pragma once

#include "charUtils.h"
#include "../interface/popup.h"

typedef enum {
	// No errors
	CC_ERROR_NO = 0,
	// The window couldn't find a display to attach to
	CC_ERROR_NODISPLAY,
	// The target OpenGL version is not supported
	CC_ERROR_GLUNSUPPORTED
} ccError;

void ccErrorString(char *dest, ccError error);
void ccAbort(const char* message);
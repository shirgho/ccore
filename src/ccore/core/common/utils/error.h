#pragma once

typedef enum {
	// No errors
	CC_ERROR_NONE = 0,
	// The window couldn't find a display to attach to
	CC_ERROR_NODISPLAY,
	// The target OpenGL version is not supported
	CC_ERROR_GLVERSION,
	// OpenGL context creation failed
	CC_ERROR_GLCONTEXT,
	// GLEW could not initialize
	CC_ERROR_GLEWINIT,
	// Resolution change failed
	CC_ERROR_RESOLUTION_CHANGE
} ccError;

const char *ccErrorString(ccError error);

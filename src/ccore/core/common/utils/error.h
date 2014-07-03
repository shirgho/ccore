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
	CC_ERROR_RESOLUTION_CHANGE,

	// Out of memory exception
	CC_ERROR_OUT_OF_MEMORY,
} ccError;

#define ccMalloc(x, size) { \
	x = malloc(size); \
	if(!x){ \
		return CC_ERROR_OUT_OF_MEMORY; \
	}}\
	

const char *ccErrorString(ccError error);

#pragma once

typedef enum {
	CC_ERROR_NONE = 0, //No errors

	//Display related
	CC_ERROR_NODISPLAY, //The window couldn't find a display to attach to
	CC_ERROR_RESOLUTION_CHANGE, //Resolution change failed

	//Window related
	CC_ERROR_WINDOWCREATION, //The window can't be created

	//OpenGL related
	CC_ERROR_GLVERSION, //The target OpenGL version is not supported
	CC_ERROR_GLCONTEXT, //OpenGL context creation failed
	CC_ERROR_GLEWINIT, //GLEW could not initialize
	CC_ERROR_CANTSWAP, //The buffers couldn't swap

	CC_ERROR_OUT_OF_MEMORY //Out of memory exception
} ccError;

#define ccMalloc(x, size) { \
	x = malloc(size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\

#define ccRealloc(x, size) { \
	x = realloc(x, size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\
	

const char *ccErrorString(ccError error);

#include "error.h"

const char *ccErrorString(ccError error)
{
	const char *str;

	switch(error){
		case CC_ERROR_NONE:
			return "No errors";
			break;
		case CC_ERROR_NODISPLAY:
			return "Could not open display";
			break;
		case CC_ERROR_GLVERSION:
			return "The current OpenGL version is not supported";
			break;
		case CC_ERROR_GLCONTEXT:
			return "The openGL context could not be created";
			break;
		case CC_ERROR_GLEWINIT:
			return "GLEW could not be initialized";
			break;
		case CC_ERROR_RESOLUTION_CHANGE:
			return "Could not change display resolution";
			break;
		default:
			return "Error message not defined";
			break;
	}
}
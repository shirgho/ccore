#include "error.h"

void ccErrorString(char *dest, ccError error)
{
	const char *str;

	switch(error){
		case ccErrorNo:
			str = "No errors";
			break;
		case ccErrorNoDisplay:
			str = "Could not open display";
			break;
		case ccErrorGLVersionUnsupported:
			str = "The current OpenGL version is not supported";
			break;
		default:
			str = "Error message not defined";
			break;
	}

	strcpy(dest, str);
}

void ccAbort(const char* message)
{
	ccShowPopup(message, "fatal error", ccPopupError);
	exit(0);
}

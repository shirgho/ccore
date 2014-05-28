#include "error.h"

void ccErrorString(char *dest, ccError error)
{
	const char *str;

	switch(error){
		case CC_ERROR_NO:
			str = "No errors";
			break;
		case CC_ERROR_NODISPLAY:
			str = "Could not open display";
			break;
		case CC_ERROR_GLUNSUPPORTED:
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
	ccShowPopup(NULL, message, "fatal error", CC_POPUP_ERROR);
	exit(0);
}

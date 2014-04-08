#include "error.h"

void ccAbort(const char* message)
{
	ccShowPopup(message, "fatal error", ccPopupError);
	exit(0);
}
#include "Error.h"

void ccAbort(const char* message)
{
	ccShowPopup("Only one window can be created!", "fatal error", ccPopupError);
	exit(0);
}
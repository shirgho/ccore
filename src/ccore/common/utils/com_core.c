#include "com_core.h"

void ccFreeAll(void)
{
	_ccErrorFree();
	_ccDirUtilsFree();

	if(_ccGamepads != NULL) ccGamepadFree();
	if(_ccDisplays != NULL) ccDisplayFree();
	ccNetFree(); // TODO: check if this exists
	if(_ccWindow != NULL) {
		if(ccGLHasContext()) ccGLFreeContext();
		ccWindowFree();
	}
}
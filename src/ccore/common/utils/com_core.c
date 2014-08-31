#include "com_core.h"

void ccFreeAll(void)
{
	_ccErrorFree();
	_ccDirUtilsFree();

	if(_gamepads != NULL) ccGamepadFree();
	if(_displays != NULL) ccDisplayFree();
	ccNetFree(); // TODO: check if this exists
	ccGLFreeContext(); // TODO: check if this exists
	if(_window != NULL) ccWindowFree();
}
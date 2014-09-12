#include <ccore/core.h>

#include <ccore/gamepad.h>
#include <ccore/display.h>
#include <ccore/net.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/file.h>
#include <ccore/net.h>

void ccFreeAll(void)
{
	_ccErrorFree();
	_ccDirUtilsFree();
	ccNetFree();

	if(_ccGamepads != NULL) ccGamepadFree();
	if(_ccDisplays != NULL) ccDisplayFree();
	//ccNetFree(); // TODO: check if this exists
	if(_ccWindow != NULL) {
		if(ccGLHasContext()) ccGLFreeContext();
		ccWindowFree();
	}
}
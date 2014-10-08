#include <ccore/core.h>

#include <ccore/gamepad.h>
#include <ccore/display.h>
#include <ccore/net.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/file.h>
#include <ccore/net.h>

void ccInitialize(void)
{
#ifdef CC_USE_GAMEPAD
	_ccGamepads = NULL;
#endif
	_ccDisplays = NULL;
#ifdef CC_USE_NET
	_ccNet = false;
#endif
	_ccWindow = NULL;
}

void ccFree(void)
{
	_ccErrorFree();

#ifdef CC_USE_FILE
	_ccFileFree();
#endif

#ifdef CC_USE_GAMEPAD
	if(_ccGamepads != NULL) ccGamepadFree();
#endif
#ifdef CC_USE_NET
	if(_ccNet) ccNetFree();
#endif
	if(_ccWindow != NULL) {
		if(ccGLContextIsActive()) ccGLContextFree();
		ccWindowFree();
	}
	if(_ccDisplays != NULL) ccDisplayFree();
}

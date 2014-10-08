#include <ccore/core.h>

#ifdef CC_USE_GAMEPAD
#include <ccore/gamepad.h>
#endif
#include <ccore/display.h>
#ifdef CC_USE_NET
#include <ccore/net.h>
#endif
#include <ccore/window.h>
#include <ccore/opengl.h>
#ifdef CC_USE_FILE
#include <ccore/file.h>
#endif

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
	if(_ccGamepads != NULL) {
		ccGamepadFree();
	}
#endif
#ifdef CC_USE_NET
	if(_ccNet) {
		ccNetFree();
	}
#endif
	if(_ccWindow != NULL) {
		if(ccGLContextIsActive()) ccGLContextFree();
		ccWindowFree();
	}
	if(_ccDisplays != NULL){
		ccDisplayFree();
	}
}

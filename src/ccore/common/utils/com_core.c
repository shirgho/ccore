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
	_ccGamepads = NULL;
	_ccDisplays = NULL;
	_ccNet = false;
	_ccWindow = NULL;
}

void ccFree(void)
{
	_ccErrorFree();
	_ccFileFree();

	if(_ccGamepads != NULL) ccGamepadFree();
	if(_ccNet) ccNetFree();
	if(_ccWindow != NULL) {
		if(ccGLHasContext()) ccGLFreeContext();
		ccWindowFree();
	}
	if(_ccDisplays != NULL) ccDisplayFree();
}
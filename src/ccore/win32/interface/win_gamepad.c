#include "win_gamepad.h"

ccError ccGamepadFind()
{
	ccAssert(_window != NULL);
	ccAssert(_gamepads == NULL);

	ccMalloc(_gamepads, sizeof(ccGamepads));
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 0x01;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 0x04;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = RIDEV_NOLEGACY;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;
	
	if(!RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(WINDOW_DATA->rid[0]))) return CC_ERROR_NOGAMEPAD;

	if(_gamepads->amount == 0) return CC_ERROR_NONE;

	return CC_ERROR_NONE;
}

ccError ccGamepadFree()
{
	ccAssert(_gamepads != NULL);
	free(_gamepads);
	return CC_ERROR_NONE;
}
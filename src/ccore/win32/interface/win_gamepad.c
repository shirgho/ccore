#include "win_gamepad.h"

ccError ccGamepadConnect()
{
	ccAssert(_window != NULL);
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 0x01;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 0x04;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE)) == TRUE?CC_ERROR_NONE:CC_ERROR_NOGAMEPAD;
}

int ccGamepadCount()
{
	return 0;
}

void ccGamepadDisconnect()
{
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = NULL;

	RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE));
}

ccGamepadEvent _generateGamepadEvent(RAWINPUT *raw)
{
	return (ccGamepadEvent){ 0 };
}
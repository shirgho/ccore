#include "win_gamepad.h"

bool ccGamepadConnect()
{
	ccAssert(_window != NULL);
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 0x01;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 0x04;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(WINDOW_DATA->rid[0])) == TRUE?true:false;
}

int ccGamepadCount()
{
	return 0;
}

void ccGamepadDisconnect()
{

}
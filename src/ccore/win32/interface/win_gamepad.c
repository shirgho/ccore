#include "win_gamepad.h"

ccError ccGamepadConnect()
{
	ccAssert(_window != NULL);
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 4;
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
	PHIDP_PREPARSED_DATA preparsedData;
	int bufferSize;

	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
	preparsedData = HeapAlloc(GetProcessHeap(), 0, bufferSize);
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, preparsedData, &bufferSize);

	return (ccGamepadEvent){ 0 };
}
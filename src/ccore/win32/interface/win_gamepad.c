#include "win_gamepad.h"

ccError ccGamepadInitialize()
{
	ccAssert(_window != NULL);

	ccMalloc(_gamepads, sizeof(ccGamepads));
	ccMalloc(_gamepads->data, sizeof(ccGamepads_win));
	GAMEPADS_DATA->preparsedDataSize = 0;
	_gamepads->amount = 0;
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 4;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE)) == TRUE?CC_ERROR_NONE:CC_ERROR_NOGAMEPAD;
}

void ccGamepadFree()
{
	ccAssert(_gamepads);

	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = NULL;

	RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE));

	if(ccGamepadCount() != 0) {
		int i;
		for(i = 0; i < ccGamepadCount(); i++) {
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->buttonCaps);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->valueCaps);
			free(_gamepads->gamepad[i].data);

			free(_gamepads->gamepad[i].button);
			free(_gamepads->gamepad[i].axis);
		}
		free(_gamepads->gamepad);
		free(((ccGamepads_win*)_gamepads->data)->preparsedData);
	}

	free(_gamepads->data);
	free(_gamepads);
}

ccGamepadEvent _generateGamepadEvent(RAWINPUT *raw)
{
	ccGamepadEvent event;
	ULONG usageLength;
	int value;
	int i;

	double newDouble;
	bool newBool;

	event.type = CC_GAMEPAD_UNHANDLED;

	// Find the current gamepad or create it
	ccGamepad *currentGamepad = NULL;

	for(i = 0; i < ccGamepadCount(); i++) {
		if(_gamepads->gamepad[i].id == (int)raw->header.hDevice) {
			currentGamepad = &_gamepads->gamepad[i];
			event.gamepadId = i;
			break;
		}
	}
	if(currentGamepad == NULL) {
		USHORT capsLength;

		_gamepads->amount++;
		if(ccGamepadCount() == 1) {
			_gamepads->gamepad = malloc(sizeof(ccGamepad));
		}
		else{
			_gamepads->gamepad = realloc(_gamepads->gamepad, ccGamepadCount() * sizeof(ccGamepad));
		}
		currentGamepad = &_gamepads->gamepad[ccGamepadCount() - 1];
		event.gamepadId = ccGamepadCount() - 1;

		// Initialize current gamepad
		GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &GAMEPADS_DATA->preparsedDataSize);
		GAMEPADS_DATA->preparsedData = malloc(GAMEPADS_DATA->preparsedDataSize);
		GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, GAMEPADS_DATA->preparsedData, &GAMEPADS_DATA->preparsedDataSize);

		currentGamepad->id = (int)raw->header.hDevice;
		currentGamepad->data = malloc(sizeof(ccGamepad_win));
		HidP_GetCaps(GAMEPADS_DATA->preparsedData, &GAMEPAD_DATA->caps);

		GAMEPAD_DATA->buttonCaps = malloc(sizeof(HIDP_BUTTON_CAPS)* GAMEPAD_DATA->caps.NumberInputButtonCaps);
		GAMEPAD_DATA->valueCaps = malloc(sizeof(HIDP_VALUE_CAPS)* GAMEPAD_DATA->caps.NumberInputValueCaps);

		capsLength = GAMEPAD_DATA->caps.NumberInputButtonCaps;
		HidP_GetButtonCaps(HidP_Input, GAMEPAD_DATA->buttonCaps, &capsLength, GAMEPADS_DATA->preparsedData);
		capsLength = GAMEPAD_DATA->caps.NumberInputValueCaps;
		HidP_GetValueCaps(HidP_Input, GAMEPAD_DATA->valueCaps, &capsLength, GAMEPADS_DATA->preparsedData);

		currentGamepad->buttonAmount = GAMEPAD_DATA->buttonCaps->Range.UsageMax - GAMEPAD_DATA->buttonCaps->Range.UsageMin + 1;
		currentGamepad->axisAmount = GAMEPAD_DATA->caps.NumberInputValueCaps;
		
		currentGamepad->button = malloc(sizeof(bool)* currentGamepad->buttonAmount);
		currentGamepad->axis = malloc(sizeof(double)* currentGamepad->axisAmount);
	}
	
	// Get buttons
	usageLength = currentGamepad->buttonAmount;
	HidP_GetUsages(HidP_Input, GAMEPAD_DATA->buttonCaps->UsagePage, 0, GAMEPADS_DATA->usage, &usageLength, GAMEPADS_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
	
	for(i = 0; i < currentGamepad->buttonAmount; i++) {
		currentGamepad->button[i] = false;
	}
	for(i = 0; i < (int)usageLength; i++)
	{
		currentGamepad->button[GAMEPADS_DATA->usage[i] - GAMEPAD_DATA->buttonCaps->Range.UsageMin] = true;
		printf("button %d\n", GAMEPADS_DATA->usage[i] - GAMEPAD_DATA->buttonCaps->Range.UsageMin);
	}

	// Get axes
	for(i = 0; i < currentGamepad->axisAmount; i++)
	{
		HidP_GetUsageValue(HidP_Input, GAMEPAD_DATA->valueCaps[i].UsagePage, 0, GAMEPAD_DATA->valueCaps[i].Range.UsageMin, &value, GAMEPADS_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		newDouble = (double)value / (GAMEPAD_DATA->valueCaps[i].PhysicalMax - GAMEPAD_DATA->valueCaps[i].PhysicalMin);
		if(GAMEPAD_DATA->valueCaps[i].PhysicalMax == 255) newDouble -= 0.5;

		if(newDouble != currentGamepad->axis[i]) {
			currentGamepad->axis[i] = newDouble;
			event.type = CC_GAMEPAD_AXIS_MOVE;
			event.axisId = i;
		}
	}

	return event;
}
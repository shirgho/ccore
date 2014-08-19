#include "win_gamepad.h"

ccError ccGamepadInitialize()
{
	ccAssert(_window != NULL);

	ccMalloc(_gamepads, sizeof(ccGamepads));
	ccMalloc(_gamepads->data, sizeof(ccGamepads_win));
	_gamepads->amount = 0;
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 4;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE)) == TRUE?CC_ERROR_NONE:CC_ERROR_NOGAMEPAD;
}

int ccGamepadCount()
{
	return 0; //the fuck
}

void ccGamepadFree()
{
	ccAssert(_gamepads);

	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = NULL;

	RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE));

	if(_gamepads->amount != 0) {
		int i;
		for(i = 0; i < _gamepads->amount; i++) {
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->buttonCaps);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->valueCaps);
			free(_gamepads->gamepad[i].data);
		}
		free(_gamepads->gamepad);
	}

	free(_gamepads->data);
	free(_gamepads);
}

ccGamepadEvent _generateGamepadEvent(RAWINPUT *raw)
{
	ULONG usageLength;

	USAGE usage[GAMEPAD_MAXBUTTONS];

	int bufferSize;
	int value;
	int i;

	// Find the current gamepad or create it
	ccGamepad *currentGamepad = NULL;

	// Get RID data
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
	GAMEPADS_DATA->preparsedData = malloc(bufferSize);
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, GAMEPADS_DATA->preparsedData, &bufferSize);

	for(i = 0; i < _gamepads->amount; i++) {
		if(_gamepads->gamepad[i].id == (int)raw->header.hDevice) {
			currentGamepad = &_gamepads->gamepad[i];
			break;
		}
	}
	if(currentGamepad == NULL) {
		USHORT capsLength;

		_gamepads->amount++;
		if(_gamepads->amount == 1) {
			_gamepads->gamepad = malloc(sizeof(ccGamepad));
		}
		else{
			_gamepads->gamepad = realloc(_gamepads->gamepad, _gamepads->amount * sizeof(ccGamepad));
		}
		currentGamepad = &_gamepads->gamepad[_gamepads->amount - 1];

		// Initialize current gamepad
		currentGamepad->id = (int)raw->header.hDevice;
		currentGamepad->data = malloc(sizeof(ccGamepad_win));
		HidP_GetCaps(GAMEPADS_DATA->preparsedData, &GAMEPAD_DATA->caps);

		GAMEPAD_DATA->buttonCaps = malloc(sizeof(HIDP_BUTTON_CAPS)* GAMEPAD_DATA->caps.NumberInputButtonCaps);
		GAMEPAD_DATA->valueCaps = malloc(sizeof(HIDP_VALUE_CAPS)* GAMEPAD_DATA->caps.NumberInputValueCaps);

		capsLength = GAMEPAD_DATA->caps.NumberInputButtonCaps;
		HidP_GetButtonCaps(HidP_Input, GAMEPAD_DATA->buttonCaps, &capsLength, GAMEPADS_DATA->preparsedData);
		capsLength = GAMEPAD_DATA->caps.NumberInputValueCaps;
		HidP_GetValueCaps(HidP_Input, GAMEPAD_DATA->valueCaps, &capsLength, GAMEPADS_DATA->preparsedData);
	}

	// Get buttons
	usageLength = GAMEPAD_DATA->buttonCaps->Range.UsageMax - GAMEPAD_DATA->buttonCaps->Range.UsageMin + 1;
	HidP_GetUsages(HidP_Input, GAMEPAD_DATA->buttonCaps->UsagePage, 0, usage, &usageLength, GAMEPADS_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);

	for(i = 0; i < (int)usageLength; i++)
	{
		printf("button %d\t%d\n", usage[i] - GAMEPAD_DATA->buttonCaps->Range.UsageMin, raw->header.hDevice);
	}

	// Get axes
	for(i = 0; i < (int)GAMEPAD_DATA->caps.NumberInputValueCaps; i++)
	{
		HidP_GetUsageValue(HidP_Input, GAMEPAD_DATA->valueCaps[i].UsagePage, 0, GAMEPAD_DATA->valueCaps[i].Range.UsageMin, &value, GAMEPADS_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		//printf("value %d\t%d\n", value, raw->header.hDevice);
	}

	// Free
	free(GAMEPADS_DATA->preparsedData);

	return (ccGamepadEvent){ 0 };
}
#include "win_gamepad.h"

ccError ccGamepadInitialize(void)
{
	ccAssert(_window != NULL);

	ccMalloc(_gamepads, sizeof(ccGamepads));
	ccMalloc(_gamepads->data, sizeof(ccGamepads_win));
	_gamepads->amount = 0;
	_gamepads->gamepad = NULL;
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].usUsage = 4;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE)) == TRUE?CC_ERROR_NONE:CC_ERROR_NOGAMEPAD;
}

void ccGamepadFree(void)
{
	ccAssert(_gamepads != NULL);
	
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_GAMEPAD].hwndTarget = NULL;
	
	RegisterRawInputDevices(&WINDOW_DATA->rid[RAWINPUT_GAMEPAD], 1, sizeof(RAWINPUTDEVICE));

	if(ccGamepadCount() != 0) {
		int i;
		for(i = 0; i < ccGamepadCount(); i++) {
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->buttonCaps);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->valueCaps);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->axisFactor);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->axisNegativeComponent);
			free(((ccGamepad_win*)_gamepads->gamepad[i].data)->preparsedData);
			free(_gamepads->gamepad[i].data);
			free(_gamepads->gamepad[i].button);
			free(_gamepads->gamepad[i].axis);
		}
		free(_gamepads->gamepad);
	}

	free(_gamepads->data);
	free(_gamepads);
}

void _generateGamepadEvents(RAWINPUT *raw)
{
	ccEvent event;
	ULONG usageLength;
	int i, j;

	int newInt;
	
	// Find the current gamepad or create it
	ccGamepad *currentGamepad = NULL;

	event.type = CC_EVENT_GAMEPAD;

	for(i = 0; i < ccGamepadCount(); i++) {
		if(_gamepads->gamepad[i].id == (int)raw->header.hDevice) {
			currentGamepad = &_gamepads->gamepad[i];
			event.gamepadEvent.id = i;
			break;
		}
	}
	if(currentGamepad == NULL) {
		USHORT capsLength;

		_gamepads->amount++;
		_gamepads->gamepad = realloc(_gamepads->gamepad, ccGamepadCount() * sizeof(ccGamepad));

		currentGamepad = &_gamepads->gamepad[ccGamepadCount() - 1];
		event.gamepadEvent.id = ccGamepadCount() - 1;

		// Initialize current gamepad
		currentGamepad->data = malloc(sizeof(ccGamepad_win));

		GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &GAMEPAD_DATA->preparsedDataSize);
		GAMEPAD_DATA->preparsedData = malloc(GAMEPAD_DATA->preparsedDataSize);
		GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, GAMEPAD_DATA->preparsedData, &GAMEPAD_DATA->preparsedDataSize);

		currentGamepad->name = "Gamepad"; //TODO: can I fetch this?
		currentGamepad->supportsVibration = false;
		currentGamepad->id = (int)raw->header.hDevice;
		HidP_GetCaps(GAMEPAD_DATA->preparsedData, &GAMEPAD_DATA->caps);

		GAMEPAD_DATA->buttonCaps = malloc(sizeof(HIDP_BUTTON_CAPS)* GAMEPAD_DATA->caps.NumberInputButtonCaps);
		GAMEPAD_DATA->valueCaps = malloc(sizeof(HIDP_VALUE_CAPS)* GAMEPAD_DATA->caps.NumberInputValueCaps);

		capsLength = GAMEPAD_DATA->caps.NumberInputButtonCaps;
		HidP_GetButtonCaps(HidP_Input, GAMEPAD_DATA->buttonCaps, &capsLength, GAMEPAD_DATA->preparsedData);
		capsLength = GAMEPAD_DATA->caps.NumberInputValueCaps;
		HidP_GetValueCaps(HidP_Input, GAMEPAD_DATA->valueCaps, &capsLength, GAMEPAD_DATA->preparsedData);

		currentGamepad->buttonAmount = GAMEPAD_DATA->buttonCaps->Range.UsageMax - GAMEPAD_DATA->buttonCaps->Range.UsageMin + 1;
		currentGamepad->axisAmount = GAMEPAD_DATA->caps.NumberInputValueCaps;
		
		currentGamepad->button = calloc(currentGamepad->buttonAmount, sizeof(bool));
		currentGamepad->axis = malloc(sizeof(int)* currentGamepad->axisAmount);
		GAMEPAD_DATA->axisFactor = malloc(sizeof(double)* currentGamepad->axisAmount);
		GAMEPAD_DATA->axisNegativeComponent = malloc(sizeof(int)* currentGamepad->axisAmount);

		for(i = 0; i < currentGamepad->axisAmount; i++) {
			GAMEPAD_DATA->axisFactor[i] = (double)(GAMEPAD_AXIS_MAX - GAMEPAD_AXIS_MIN) / (GAMEPAD_DATA->valueCaps[i].PhysicalMax - GAMEPAD_DATA->valueCaps[i].PhysicalMin);
			GAMEPAD_DATA->axisNegativeComponent[i] = ((GAMEPAD_DATA->valueCaps[i].PhysicalMax - GAMEPAD_DATA->valueCaps[i].PhysicalMin) >> 1) - GAMEPAD_DATA->valueCaps[i].PhysicalMin;
		}

		event.gamepadEvent.type = CC_GAMEPAD_CONNECT;
		_ccEventStackPush(event);
	}
	
	// Get buttons
	usageLength = currentGamepad->buttonAmount;
	HidP_GetUsages(HidP_Input, GAMEPAD_DATA->buttonCaps->UsagePage, 0, GAMEPADS_DATA->usage, &usageLength, GAMEPAD_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
	
	for(i = 0; i < (int)usageLength; i++)
	{
		int index = GAMEPADS_DATA->usage[i] - GAMEPAD_DATA->buttonCaps->Range.UsageMin;
		if(currentGamepad->button[index] == false) {
			currentGamepad->button[index] = true;
			
			event.gamepadEvent.type = CC_GAMEPAD_BUTTON_DOWN;
			event.gamepadEvent.buttonId = index;
			_ccEventStackPush(event);
		}
	}
	for(i = 0; i < currentGamepad->buttonAmount; i++) {
		if(currentGamepad->button[i] == true) {
			for(j = 0; j < (int)usageLength; j++) {
				if(currentGamepad->button[GAMEPADS_DATA->usage[j] - GAMEPAD_DATA->buttonCaps->Range.UsageMin] == true) {
					goto pressed;
				}
			}
			currentGamepad->button[i] = false;

			event.gamepadEvent.type = CC_GAMEPAD_BUTTON_UP;
			event.gamepadEvent.buttonId = i;
			_ccEventStackPush(event);
		}
	pressed:;
	}

	// Get axes
	usageLength = currentGamepad->axisAmount;

	for(i = 0; i < currentGamepad->axisAmount; i++)
	{
		HidP_GetUsageValue(HidP_Input, GAMEPAD_DATA->valueCaps[i].UsagePage, 0, GAMEPAD_DATA->valueCaps[i].NotRange.Usage, &newInt, GAMEPAD_DATA->preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		newInt = (int)((newInt - GAMEPAD_DATA->axisNegativeComponent[i]) * GAMEPAD_DATA->axisFactor[i]);
		if(newInt < GAMEPAD_AXIS_MIN) {
			newInt = GAMEPAD_AXIS_MIN;
		}
		else if(newInt > GAMEPAD_AXIS_MAX) {
			newInt = GAMEPAD_AXIS_MAX;
		}

		if(newInt != currentGamepad->axis[i]) {
			currentGamepad->axis[i] = newInt;
			event.gamepadEvent.type = CC_GAMEPAD_AXIS_MOVE;
			event.gamepadEvent.axisId = i;
			_ccEventStackPush(event);
		}
	}
}
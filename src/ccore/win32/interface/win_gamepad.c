#include "win_gamepad.h"

ccError ccGamepadInitialize()
{
	ccAssert(_window != NULL);

	ccMalloc(_gamepads, sizeof(ccGamepads));
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

	free(_gamepads);
}

ccGamepadEvent _generateGamepadEvent(RAWINPUT *raw)
{
	HANDLE heap;
	PHIDP_PREPARSED_DATA preparsedData;
	HIDP_CAPS caps;
	USHORT capsLength;
	ULONG usageLength;

	USAGE usage[GAMEPAD_MAXBUTTONS];

	PHIDP_BUTTON_CAPS buttonCaps;
	PHIDP_VALUE_CAPS valueCaps;

	int bufferSize;
	int buttonCount;
	int value;
	int i;

	// Find the current gamepad or create it
	ccGamepad *currentGamepad = NULL;

	for(i = 0; i < _gamepads->amount; i++) {
		if(_gamepads->gamepad[i].id == (int)raw->header.hDevice) {
			currentGamepad = &_gamepads->gamepad[i];
			break;
		}
	}
	if(currentGamepad == NULL) {
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
	}

	printf("%d\n", currentGamepad);

	heap = GetProcessHeap();

	// Get RID data
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
	preparsedData = malloc(bufferSize);
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, preparsedData, &bufferSize);

	// Get controller data
	HidP_GetCaps(preparsedData, &caps);
	buttonCaps = HeapAlloc(heap, 0, sizeof(HIDP_BUTTON_CAPS)* caps.NumberInputButtonCaps);

	// Get button data
	capsLength = caps.NumberInputButtonCaps;
	HidP_GetButtonCaps(HidP_Input, buttonCaps, &capsLength, preparsedData);
	buttonCount = buttonCaps->Range.UsageMax - buttonCaps->Range.UsageMin + 1;
	
	// Get value data
	valueCaps = HeapAlloc(heap, 0, sizeof(HIDP_VALUE_CAPS)* caps.NumberInputValueCaps);
	capsLength = caps.NumberInputValueCaps;
	HidP_GetValueCaps(HidP_Input, valueCaps, &capsLength, preparsedData);

	// Get buttons
	usageLength = buttonCount;
	HidP_GetUsages(HidP_Input, buttonCaps->UsagePage, 0, usage, &usageLength, preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);

	for(i = 0; i < (int)usageLength; i++)
	{
		printf("button %d\t%d\n", usage[i] - buttonCaps->Range.UsageMin, raw->header.hDevice);
	}

	// Get axes
	for(i = 0; i < (int)caps.NumberInputValueCaps; i++)
	{
		HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0, valueCaps[i].Range.UsageMin, &value, preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		//printf("value %d\t%d\n", value, raw->header.hDevice);
	}

	// Free
	free(preparsedData);

	return (ccGamepadEvent){ 0 };
}
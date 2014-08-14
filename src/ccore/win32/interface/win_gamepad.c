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
	int i;

	heap = GetProcessHeap();

	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
	preparsedData = HeapAlloc(heap, 0, bufferSize);
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

	for(i = 0; i < usageLength; i++)
	{
		char *str = malloc(500);
		HidD_GetManufacturerString(HidP_Input, &str, 500);
		printf("button %d @ %s\n", usage[i] - buttonCaps->Range.UsageMin, str);
	}

	// Free
	HeapFree(heap, 0, preparsedData);
	HeapFree(heap, 0, buttonCaps);

	return (ccGamepadEvent){ 0 };
}
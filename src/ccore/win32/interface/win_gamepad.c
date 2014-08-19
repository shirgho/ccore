#include "win_gamepad.h"

ccError ccGamepadInitialize()
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

void ccGamepadFree()
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
	int value;
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
		printf("button %d\t%d\n", usage[i] - buttonCaps->Range.UsageMin, raw->header.hDevice);
	}

	// Get axes
	for(i = 0; i < caps.NumberInputValueCaps; i++)
	{
		HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0, valueCaps[i].Range.UsageMin, &value, preparsedData, raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		printf("value %d\t%d\n", value, raw->header.hDevice);
	}

	// Free
	HeapFree(heap, 0, preparsedData);
	HeapFree(heap, 0, buttonCaps);

	return (ccGamepadEvent){ 0 };
}
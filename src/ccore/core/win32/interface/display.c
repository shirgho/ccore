#include "../../common/interface/display.h"

int ccGetDisplayAmount()
{
	ccAssert(_displays != NULL);

	return _displays->amount;
}

ccDisplay *ccGetDisplay(int index)
{
	ccAssert(_displays != NULL);
	ccAssert(index >= 0 && index < _displays->amount);

	return &_displays->display[index];
}

ccDisplay *ccGetDefaultDisplay()
{
	ccAssert(_displays != NULL);

	return &_displays->display[_displays->primary];
}

ccError ccFindDisplays()
{
	DISPLAY_DEVICE device;
	DISPLAY_DEVICE display;
	DEVMODE dm;
	ccDisplay *currentDisplay;
	ccDisplayData buffer;
	ccDisplayData initialBuffer;
	int deviceCount = 0;
	int displayCount;
	int i;

	ccAssert(_displays == NULL);

	ccMalloc(_displays, sizeof(ccDisplays));

	if(_displays == NULL) return CC_ERROR_OUT_OF_MEMORY;

	dm.dmSize = sizeof(dm);
	device.cb = sizeof(DISPLAY_DEVICE);
	display.cb = sizeof(DISPLAY_DEVICE);
	_displays->amount = 0;

	while(EnumDisplayDevices(NULL, deviceCount, &device, 0)) {
		displayCount = 0;

		while(EnumDisplayDevices(device.DeviceName, displayCount, &display, 0)) {
			_displays->amount++;

			if(_displays->amount == 1) {
				ccMalloc(_displays->display, sizeof(ccDisplay));
			}
			else{
				ccRealloc(_displays->display, sizeof(ccDisplay)*_displays->amount);
			}

			EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &dm);

			currentDisplay = &_displays->display[_displays->amount - 1];

			ccMalloc(currentDisplay->gpuName, CC_MAXDEVICENAMESIZE);
			ccMalloc(currentDisplay->monitorName, CC_MAXDEVICENAMESIZE);
			ccMalloc(currentDisplay->deviceName, CC_MAXDEVICENAMESIZE);

			memcpy(currentDisplay->gpuName, device.DeviceString, 128);
			memcpy(currentDisplay->monitorName, display.DeviceString, 128);
			memcpy(currentDisplay->deviceName, display.DeviceName, 128);
			ccStrTrimToChar(currentDisplay->deviceName, '\\', false);

			ccStrTrim(currentDisplay->gpuName);
			ccStrTrim(currentDisplay->monitorName);
			ccStrTrim(currentDisplay->deviceName);

			currentDisplay->x = dm.dmPosition.x;
			currentDisplay->y = dm.dmPosition.y;

			currentDisplay->amount = 0;

			initialBuffer.bitDepth = dm.dmBitsPerPel;
			initialBuffer.refreshRate = dm.dmDisplayFrequency;
			initialBuffer.width = dm.dmPelsWidth;
			initialBuffer.height = dm.dmPelsHeight;

			i = 0;
			while(EnumDisplaySettings(device.DeviceName, i, &dm)) {
				i++;

				buffer.bitDepth = dm.dmBitsPerPel;
				buffer.refreshRate = dm.dmDisplayFrequency;
				buffer.width = dm.dmPelsWidth;
				buffer.height = dm.dmPelsHeight;

				if(ccResolutionExists(currentDisplay, &buffer)) continue;

				if(currentDisplay->amount == 0) {
					ccMalloc(currentDisplay->resolution, sizeof(ccDisplayData));
				}
				else{
					ccRealloc(currentDisplay->resolution, sizeof(ccDisplayData)*(currentDisplay->amount + 1));
				}

				if(memcmp(&initialBuffer, &buffer, sizeof(ccDisplayData)) == 0) {
					currentDisplay->current = currentDisplay->amount;
					currentDisplay->initial = currentDisplay->current;
				}
				memcpy(&currentDisplay->resolution[currentDisplay->amount], &buffer, sizeof(ccDisplayData));

				currentDisplay->amount++;
			}

			if(currentDisplay->x == 0 && currentDisplay->y == 0) _displays->primary = _displays->amount - 1;

			displayCount++;

		}
		deviceCount++;
	}

	return CC_ERROR_NONE;
}

ccError ccFreeDisplays() {
	int i;

	ccAssert(_displays != NULL);

	for(i = 0; i < _displays->amount; i++) {
		free(_displays->display[i].gpuName);
		free(_displays->display[i].monitorName);
		free(_displays->display[i].deviceName);
		free(_displays->display[i].resolution);
	}
	free(_displays->display);
	free(_displays);

	return CC_ERROR_NONE;
}

ccError ccRevertDisplays()
{
	int i;

	ccAssert(_displays != NULL);

	for(i = 0; i < _displays->amount; i++){
		ccSetResolution(_displays->display + i, CC_DEFAULT_RESOLUTION);
	}

	return CC_ERROR_NONE;
}

ccRect ccGetDisplayRect(ccDisplay *display)
{
	ccRect rect;

	ccAssert(display != NULL);

	rect.x = display->x;
	rect.y = display->y;
	rect.width = display->resolution[display->current].width;
	rect.height = display->resolution[display->current].height;
	return rect;
}

bool ccResolutionExists(ccDisplay *display, ccDisplayData *resolution)
{
	int i;

	ccAssert(display != NULL);

	for(i = 0; i < display->amount; i++) {
		if(memcmp(&display->resolution[i], resolution, sizeof(ccDisplayData)) == 0) {
			return true;
		}
	}

	return false;
}

ccError ccSetResolution(ccDisplay *display, int resolutionIndex)
{
	DEVMODE devMode;
	ccDisplayData displayData;

	ccAssert(display != NULL);
	ccAssert(resolutionIndex < display->amount);

	if(resolutionIndex == CC_DEFAULT_RESOLUTION) resolutionIndex = display->initial;
	if(resolutionIndex == display->current) return CC_ERROR_NONE;

	ZeroMemory(&devMode, sizeof(DEVMODE));
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(display->deviceName, ENUM_CURRENT_SETTINGS, &devMode);

	displayData = display->resolution[resolutionIndex];

	devMode.dmPelsWidth = displayData.width;
	devMode.dmPelsHeight = displayData.height;
	devMode.dmBitsPerPel = displayData.bitDepth;
	devMode.dmDisplayFrequency = displayData.refreshRate;
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

	if(ChangeDisplaySettingsEx(display->deviceName, &devMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL) {
		return CC_ERROR_RESOLUTION_CHANGE;
	}

	display->current = resolutionIndex;

	return CC_ERROR_NONE;
}
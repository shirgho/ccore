#include <ccore/display.h>

bool ccDisplayResolutionEqual(ccDisplayData *resolutionA, ccDisplayData *resolutionB)
{
	return (resolutionA->bitDepth == resolutionB->bitDepth && resolutionA->height == resolutionB->height &&
		resolutionA->refreshRate == resolutionB->refreshRate && resolutionA->width == resolutionB->width);
}

ccRect ccDisplayGetRect(ccDisplay *display)
{
	ccAssert(display != NULL);

	return (ccRect){ display->x, display->y, display->resolution[display->current].width, display->resolution[display->current].height };
}

bool ccDisplayResolutionExists(ccDisplay *display, ccDisplayData *resolution)
{
	int i;

	ccAssert(display != NULL);

	for(i = 0; i < display->amount; i++) {
		if(ccDisplayResolutionEqual(&display->resolution[i], resolution)) {
			return true;
		}
	}

	return false;
}

ccError ccDisplayRevertModes(void)
{
	int i;
	ccError output;

	ccAssert(_displays != NULL);

	for(i = 0; i < _displays->amount; i++){
		output = ccDisplaySetResolution(_displays->display + i, CC_DEFAULT_RESOLUTION);
		if(output != CC_ERROR_NONE){
			return output;
		}
	}

	return CC_ERROR_NONE;
}

ccDisplay *ccDisplayGetDefault(void)
{
	ccAssert(_displays != NULL);
	ccAssert(_displays->display != NULL);

	return _displays->display + _displays->primary;
}

ccDisplay *ccDisplayGet(int index)
{
	ccAssert(_displays != NULL);
	ccAssert(index >= 0 && index < _displays->amount);

	return _displays->display + index;
}

int ccDisplayGetAmount(void)
{
	ccAssert(_displays != NULL);

	return _displays->amount;
}
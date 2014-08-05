#include <ccore/display.h>

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

ccError ccRevertDisplays()
{
	int i;
	ccError output;

	ccAssert(_displays != NULL);

	for(i = 0; i < _displays->amount; i++){
		output = ccSetResolution(_displays->display + i, CC_DEFAULT_RESOLUTION);
		if(output != CC_ERROR_NONE){
			return output;
		}
	}

	return CC_ERROR_NONE;
}

ccDisplay *ccGetDefaultDisplay()
{
	ccAssert(_displays != NULL);
	ccAssert(_displays->display != NULL);

	return _displays->display + _displays->primary;
}

ccDisplay *ccGetDisplay(int index)
{
	ccAssert(_displays != NULL);
	ccAssert(index >= 0 && index < _displays->amount);

	return _displays->display + index;
}

int ccGetDisplayAmount()
{
	ccAssert(_displays != NULL);

	return _displays->amount;
}
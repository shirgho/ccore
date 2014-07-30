#include "../../common/interface/display.h"

int ccGetDisplayAmount()
{
	ccAssert(_displays != NULL);
	ccAssert(_displays->amount != 0);

	return _displays->amount;
}

ccDisplay *ccGetDisplay(int index)
{
	ccAssert(_displays != NULL);
	ccAssert(index >= 0 && index < _displays->amount);

	return _displays->display + index;
}

ccDisplay *ccGetDefaultDisplay()
{
	ccAssert(_displays != NULL);
	ccAssert(_displays->display != NULL);

	return _displays->display + _displays->primary;
}

static bool ccXFindDisplaysXinerama(Display *display, char *displayName)
{
	int i, j, k, displayNameLength, eventBase, errorBase;
	bool foundCrtc;
	unsigned int vTotal;
	ccDisplay *currentDisplay;
	ccDisplayData currentResolution;
	Window root;
	XRRScreenResources *resources;
	XRROutputInfo *outputInfo;
	XRRCrtcInfo *crtcInfo;

	if(!XineramaQueryExtension(display, &eventBase, &errorBase) || !XineramaIsActive(display)){
		ccPrintString("Xinerama not supported or active\n");
		return false;
	}

	currentResolution.bitDepth = -1;
	_displays->primary = 0;

	root = RootWindow(display, 0);
	resources = XRRGetScreenResources(display, root);

	for(i = 0; i < resources->noutput; i++){
		outputInfo = XRRGetOutputInfo(display, resources, resources->outputs[i]);
		/* Ignore disconnected devices */
		if(outputInfo->connection != 0){
			ccPrintString("X: Ignored disconnected display %d\n", i);
			continue;
		}

		_displays->amount++;
		if(_displays->amount == 1){
			ccMalloc(_displays->display, sizeof(ccDisplay));
		}else{
			ccRealloc(_displays->display, sizeof(ccDisplay) * _displays->amount);
		}
		currentDisplay = _displays->display + _displays->amount - 1;

		displayNameLength = strlen(displayName);
		currentDisplay->XDisplayName = malloc(displayNameLength + 1);
		memcpy(currentDisplay->XDisplayName, displayName, displayNameLength);

		currentDisplay->monitorName = malloc(outputInfo->nameLen + 1);
		memcpy(currentDisplay->monitorName, outputInfo->name, outputInfo->nameLen);

		currentDisplay->XDisplayName[displayNameLength] = '\0';
		currentDisplay->gpuName = "Undefined";

		foundCrtc = false;
		for(j = 0; j < resources->ncrtc; j++){	
			if(resources->crtcs[j] != outputInfo->crtc){
				continue;
			}
			crtcInfo = XRRGetCrtcInfo(display, resources, resources->crtcs[j]);
			if(crtcInfo->mode == None){
				continue;
			}

			currentDisplay->x = crtcInfo->x;
			currentDisplay->y = crtcInfo->y;
			currentDisplay->XOldMode = crtcInfo->mode;
			foundCrtc = true;

			XRRFreeCrtcInfo(crtcInfo);
			break;
		}
		if(!foundCrtc){
			currentDisplay->x = -1;
			currentDisplay->y = -1;
		}

		currentDisplay->XineramaScreen = i;
		currentDisplay->XScreen = 0;
		currentDisplay->XOutput = resources->outputs[i];
		currentDisplay->current = 0;
		currentDisplay->amount = 0;

		for(j = 0; j < outputInfo->nmode; j++){
			for(k = 0; k < resources->nmode; k++){
				if(outputInfo->modes[j] == resources->modes[k].id){
					vTotal = resources->modes[k].vTotal;
					if(resources->modes[k].modeFlags & RR_DoubleScan){
						vTotal <<= 1;
					}
					if(resources->modes[k].modeFlags & RR_Interlace){
						vTotal >>= 1;
					}

					currentResolution.XMode = outputInfo->modes[j];
					currentResolution.refreshRate = resources->modes[k].dotClock / (resources->modes[k].hTotal * vTotal);
					currentResolution.width = resources->modes[k].width;
					currentResolution.height = resources->modes[k].height;

					currentDisplay->amount++;
					if(currentDisplay->amount == 1){
						ccMalloc(currentDisplay->resolution, sizeof(ccDisplayData));
					}else{
						ccRealloc(currentDisplay->resolution, sizeof(ccDisplayData) * currentDisplay->amount);
					}
					memcpy(currentDisplay->resolution + (currentDisplay->amount - 1), &currentResolution, sizeof(ccDisplayData));
					break;
				}
			}
		}

		XRRFreeOutputInfo(outputInfo);
	}

	XRRFreeScreenResources(resources);

	return true;
}

ccError ccFindDisplays()
{
	char displayName[64];
	DIR *dir;
	struct dirent *direntry;
	Display *display;

	ccAssert(_displays == NULL);

	ccMalloc(_displays, sizeof(ccDisplays));
	_displays->amount = 0;

#ifdef LINUX
	dir = opendir("/tmp/.X11-unix");
#endif
	ccAssert(dir != NULL);

	while((direntry = readdir(dir)) != NULL){
		if(direntry->d_name[0] != 'X'){
			continue;
		}
		snprintf(displayName, 64, ":%s", direntry->d_name + 1);
		ccPrintString("X: Found root display %s\n", displayName);
		display = XOpenDisplay(displayName);
		if(display != NULL){
			if(!ccXFindDisplaysXinerama(display, displayName)){
				//ccXFindDisplaysXrandr(display, displayName);
				return CC_ERROR_NODISPLAY;
			}		
			ccPrintString("X: %d displays found\n", _displays->amount);
			XCloseDisplay(display);
		}
	}

	return CC_ERROR_NONE;
}

ccError ccFreeDisplays()
{
	ccAssert(_displays != NULL);

	/*	int i;

		for(i = 0; i < _displays->amount; i++){
		if(_displays->display + i != NULL){
		free(_displays->display[i].gpuName);
		free(_displays->display[i].monitorName);
		free(_displays->display[i].resolution);
		}
		} */
	free(_displays->display);

	return CC_ERROR_NONE;
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
	int minX, minY, maxX, maxY;
	ccDisplayData *displayData;
	Display *XDisplay;
	Window root;
	XRRScreenResources *resources;
	XRROutputInfo *outputInfo;
	XRRCrtcInfo *crtcInfo;

	ccAssert(display != NULL);
	ccAssert(resolutionIndex < display->amount);

	XDisplay = XOpenDisplay(display->XDisplayName);
	root = RootWindow(XDisplay, display->XScreen);
	XGrabServer(XDisplay);

	resources = XRRGetScreenResources(XDisplay, root);
	if(!resources){
		ccPrintString("X: Couldn't get screen resources");
		return CC_ERROR_RESOLUTION_CHANGE;
	}
	outputInfo = XRRGetOutputInfo(XDisplay, resources, display->XOutput);
	if(!outputInfo || outputInfo->connection == RR_Disconnected){
		ccPrintString("X: Couldn't get output info");
		XRRFreeScreenResources(resources);
		return CC_ERROR_RESOLUTION_CHANGE;
	}
	crtcInfo = XRRGetCrtcInfo(XDisplay, resources, outputInfo->crtc);
	if(!crtcInfo){
		ccPrintString("X: Couldn't get crtc info");
		XRRFreeScreenResources(resources);
		XRRFreeOutputInfo(outputInfo);
		return CC_ERROR_RESOLUTION_CHANGE;
	}

	if(resolutionIndex != CC_DEFAULT_RESOLUTION){
		displayData = display->resolution + resolutionIndex;
		if(display->resolution->width == displayData->width && display->resolution->height == displayData->height){
			return CC_ERROR_NONE;
		}

		if(display->resolution->width <= 0 || display->resolution->height <= 0){
			ccPrintString("Error: Resolution supplied not valid\n");
			return CC_ERROR_RESOLUTION_CHANGE;
		}

		if(!XRRGetScreenSizeRange(XDisplay, root, &minX, &minY, &maxX, &maxY)){
			ccPrintString("X: Unable to get screen size range\n");
			return CC_ERROR_RESOLUTION_CHANGE;
		}

		if(displayData->width < minX || displayData->height < minY){
			ccPrintString("X: Unable to set size of screen below the minimum of %dx%d\n", minX, minY);
			return CC_ERROR_RESOLUTION_CHANGE;
		} else if(displayData->width > maxX || displayData->height > maxY){
			ccPrintString("X: Unable to set size of screen above the maximum of %dx%d\n", maxX, maxY);
			return CC_ERROR_RESOLUTION_CHANGE;
		}

		ccPrintString("X: Setting display %d to %dx%d\n", display->XScreen, displayData->width, displayData->height);
		XRRSetCrtcConfig(XDisplay, resources, outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, displayData->XMode, crtcInfo->rotation, &display->XOutput, 1);
	}else{
		ccPrintString("X: Reverting display %d\n", display->XScreen);
		XRRSetCrtcConfig(XDisplay, resources, outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, display->XOldMode, crtcInfo->rotation, &display->XOutput, 1);
	}

	XRRFreeScreenResources(resources);
	XRRFreeOutputInfo(outputInfo);
	XRRFreeCrtcInfo(crtcInfo);

	XSync(XDisplay, False);
	XUngrabServer(XDisplay);
	XCloseDisplay(XDisplay);

	return CC_ERROR_NONE;
}

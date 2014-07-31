#pragma once

#include "core.h"
#include "error.h"
#include "types.h"
#include "assert.h"

#ifdef X11
#include "../../src/ccore/x11/interface/display.h"
#elif defined WIN32
#include "../../src/ccore/win32/interface/display.h"
#endif

#define CC_DEFAULT_RESOLUTION -1

//stores display properties
typedef struct {
	int width, height, refreshRate, bitDepth;
#ifdef LINUX
	RRMode XMode;
#endif
} ccDisplayData;

//display
typedef struct {
	ccDisplayData *resolution;
	int x, y;
	unsigned short amount, current, initial;
	char *gpuName;
	char *monitorName;

#ifdef WIN32
	char* deviceName;
#endif

#ifdef LINUX
	char *XDisplayName;
	int XScreen, XineramaScreen;
	RROutput XOutput;
	RRMode XOldMode;
#endif

} ccDisplay;

//list of all displays currently connected and active
typedef struct {
	ccDisplay* display;
	unsigned short amount, primary;
} ccDisplays;

//only access through getters
ccDisplays *_displays;

#define ccGetResolutionCurrent(display) (&display->resolution[display->current])
#define ccGetResolution(display, index) (&display->resolution[index])
#define ccGetResolutionAmount(display) display->amount

//getters
int ccGetDisplayAmount();
ccDisplay *ccGetDisplay(int index);
ccDisplay *ccGetDefaultDisplay();

//resolution
ccError ccSetResolution(ccDisplay *display, int resolutionIndex);
bool ccResolutionExists(ccDisplay *display, ccDisplayData *resolution);

//display
ccError ccFindDisplays(); //get all displays currently connected and active
ccError ccFreeDisplays();
ccError ccRevertDisplays();
ccRect ccGetDisplayRect(ccDisplay *display);

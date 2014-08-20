//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                //
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License as published by        //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

#pragma once

#include <string.h>

#include "core.h"

#include "error.h"
#include "types.h"
#include "assert.h"

#define CC_DEFAULT_RESOLUTION -1

#ifdef __cplusplus
extern "C"
{
#endif

// Stores display properties
typedef struct {
	int width, height, refreshRate, bitDepth;

	void *data;
} ccDisplayData;

// Display
typedef struct {
	ccDisplayData *resolution;
	int x, y;
	unsigned short amount, current, initial;
	char *gpuName;
	char *monitorName;

	char *deviceName;
	void *data;
} ccDisplay;

// List of all displays currently connected and active
typedef struct {
	ccDisplay* display;
	unsigned short amount, primary;
} ccDisplays;

// Only access through getters
ccDisplays *_displays;

#define ccDisplayGetResolutionCurrent(display) (&display->resolution[display->current])
#define ccDisplayGetResolution(display, index) (&display->resolution[index])
#define ccDisplayGetResolutionAmount(display) display->amount

// Getters
int ccDisplayGetAmount();
ccDisplay *ccDisplayGet(int index);
ccDisplay *ccDisplayGetDefault();

// Resolution
ccError ccDisplaySetResolution(ccDisplay *display, int resolutionIndex);
bool ccDisplayResolutionExists(ccDisplay *display, ccDisplayData *resolution);

// Display
ccError ccDisplayInitialize(); // Get all displays currently connected and active
ccError ccDisplayFree();
ccError ccDisplayRevertModes();
ccRect ccDisplayGetRect(ccDisplay *display);

#ifdef __cplusplus
}
#endif

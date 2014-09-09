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

#include <stdarg.h>

#include "core.h"

#include "error.h"
#include "display.h"
#include "types.h"
#include "event.h"

#define CC_FULLSCREEN_CURRENT_DISPLAY 0

#ifdef __cplusplus
extern "C"
{
#endif

// A window can contain multiple flags to determine the layout and functionality
typedef enum {
	CC_WINDOW_FLAG_NORESIZE = 1,
	CC_WINDOW_FLAG_ALWAYSONTOP = 2,
	CC_WINDOW_FLAG_NOBUTTONS = 4,
	CC_WINDOW_FLAG_NORAWINPUT = 8
} ccWindowFlag;

// The window struct
typedef struct {
	ccRect rect;
	ccPoint mouse;
	ccEvent event;
	ccDisplay *display;
	bool supportsRawInput;

	void *data;
} ccWindow;

// Only access through getters
ccWindow *_ccWindow;

#define ccWindowSupportsRawInput() _ccWindow->supportsRawInput

// Window functions
ccReturn ccWindowCreate(ccRect rect, const char *title, int flags);
ccReturn ccWindowFree(void);
bool ccWindowPollEvent(void); // Poll an event from the events that currently need to be processed in the window
ccReturn ccWindowResizeMove(ccRect rect);
ccReturn ccWindowCenter(void);

ccReturn ccWindowSetWindowed(void);
ccReturn ccWindowSetMaximized(void);
ccReturn ccWindowSetFullscreen(int displayCount, ...);

// Getters
ccEvent ccWindowGetEvent(void);
ccRect ccWindowGetRect(void);
ccPoint ccWindowGetMouse(void);
ccDisplay *ccWindowGetDisplay(void);
bool ccWindowExists(void);

// Usually for internal use only, finds the display the window currently is in
void ccWindowUpdateDisplay(void);

#ifdef __cplusplus
}
#endif

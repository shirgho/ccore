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

#include "core.h"
#include "error.h"
#include "display.h"
#include "types.h"
#include "event.h"

#include <stdarg.h>

#ifdef X11
#include "../../src/ccore/x11/interface/x11_window.h"
#elif defined WIN32
#include "../../src/ccore/win32/interface/win_window.h"
#endif

//a window can contain multiple flags to determine the layout and functionality
typedef enum {
	CC_WINDOW_FLAG_NORESIZE = 1, //note: this cannot be ensured in linux, the WM ultimately decides whether this is allowed
	CC_WINDOW_FLAG_ALWAYSONTOP = 2,
	CC_WINDOW_FLAG_NOBUTTONS = 4
} ccWindowFlag;

//do not use this enum!
#ifdef WIN32
typedef enum {
	CC_WIN32_EVENT_RESIZED = 1,
	CC_WIN32_EVENT_FOCUS_GAINED = 2,
	CC_WIN32_EVENT_FOCUS_LOST = 4
} ccWin32Event;
#endif

//the window struct
typedef struct {
	//Note: dimensions subtract border size
	float aspect;
	ccRect rect;
	ccPoint mouse;
	ccEvent event;
	ccDisplay *display;

#ifdef X11
	Display *XDisplay; 
	Window XWindow;
	int XScreen;
	int windowFlags;
	bool resizable;
	GLXContext XContext;
#endif

#ifdef WIN32
	HDC hdc;
	MSG msg;
	HWND winHandle;
	HGLRC renderContext;
	int specialEvents;
	LONG style;
	RAWINPUTDEVICE rid[NRAWINPUTDEVICES];
	LPBYTE lpb;
	UINT lpbSize;
	UINT dwSize;
#endif

} ccWindow;

//only access through getters
ccWindow *_window;

//getters
ccEvent ccGetEvent();
ccRect ccGetWindowRect();
ccPoint ccGetWindowMouse();
ccDisplay *ccGetWindowDisplay();
bool ccWindowExists();

//window functions
ccError ccNewWindow(ccRect rect, const char *title, int flags);
ccError ccFreeWindow();
bool ccPollEvent(); //poll an event from the events that currently need to be processed in the window
ccError ccResizeMoveWindow(ccRect rect, bool addBorder); //addBorder indicates whether the border size is included in the rect or whether it has to be added
ccError ccCenterWindow();

ccError ccSetWindowed();
ccError ccSetMaximized();
ccError ccSetFullscreen(int displayCount, ...);

//usually for internal use only
void ccUpdateWindowDisplay();

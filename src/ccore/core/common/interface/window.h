#pragma once

#include "../core.h"
#include "../utils/error.h"
#include "display.h"
#include "../../common/types.h"
#include "../../common/interface/event.h"

#ifdef X11
#include "../../x11/interface/window.h"
#elif defined WIN32
#include "../../win32/interface/window.h"
#endif


//the way a window is shown. Only one window mode can be active at a time
typedef enum {
	CC_WINDOW_MODE_FULLSCREEN,
	CC_WINDOW_MODE_MAXIMIZED,
	CC_WINDOW_MODE_WINDOW,
} ccWindowMode;

//a window can contain multiple flags to determine the layout and functionality
typedef enum {
	CC_WINDOW_FLAG_NORESIZE = 1,
	CC_WINDOW_FLAG_ALWAYSONTOP = 2,
	CC_WINDOW_FLAG_NOBUTTONS = 4
} ccWindowFlag;

//the window struct
typedef struct {
	//Note: dimensions subtract border size
	float aspect;
	ccRect rect;
	ccPoint mouse;
	ccEvent event;
	ccDisplay *display;

#ifdef LINUX
	Display *XDisplay; 
	Window XWindow;
	int XScreen;
	GLXContext XContext;
#endif

#ifdef WIN32
	HDC hdc;
	MSG msg;
	HWND winHandle;
	HGLRC renderContext;
	bool sizeChanged;
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
bool ccWindowExists();

//window functions
ccError ccNewWindow(ccRect rect, const char *title, int flags);
ccError ccFreeWindow();
bool ccPollEvent(); //poll an event from the events that currently need to be processed in the window
ccError ccChangeWM(ccWindowMode mode);
ccError ccResizeMoveWindow(ccRect rect, bool addBorder); //addBorder indicates whether the border size is included in the rect or whether it has to be added
ccError ccCenterWindow();

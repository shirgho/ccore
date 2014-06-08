#pragma once

#include "../core.h"
#include "../utils/error.h"

#ifdef X11
#include "../../x11/interface/window.h"
#elif defined GTK
#include "../../gtk/interface/window.h"
#elif defined WIN32
#include "../../win32/interface/window.h"
#endif

//stores display properties
typedef struct {
	int width, height, refreshRate, bitDepth;
} ccDisplayData;

//a display (often a monitor)
typedef struct {
	//current display configuration
	ccDisplayData *resolution;
	int x, y, amount, current;
	char *gpuName;
	char *monitorName;
	
#ifdef WIN32
	char* deviceName;
#endif

#ifdef LINUX
	int XScreen, XineramaScreen;
#endif

} ccDisplay;

//list of all displays currently connected and active
typedef struct {
	ccDisplay* display;
	int amount;
	int primary;
} ccDisplays;

//the way a window is shown. Only one window mode can be active at a time
typedef enum {
	CC_WINDOW_MODE_FULLSCREEN,
	CC_WINDOW_MODE_MAXIMIZED,
	CC_WINDOW_MODE_WINDOW,
	CC_WINDOW_MODE_MINIMIZED
} ccWindowMode;

//a window can contain multiple flags to determine the layout and functionality
typedef enum {
	CC_WINDOW_FLAG_NORESIZE = 1,
	CC_WINDOW_FLAG_ALWAYSONTOP = 2,
	CC_WINDOW_FLAG_NOBUTTONS = 4
} ccWindowFlag;

//the window struct
typedef struct _ccWindow {
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
#endif

} ccWindow;

#define ccGetResolutionCurrent(display) (&display->resolution[display->current])
#define ccGetResolution(display, index) (&display->resolution[index])
#define ccGetResolutionAmount(display) display->amount

//ccWindow related
ccWindow* ccNewWindow(ccRect rect, const char *title, int flags);
void ccFreeWindow(ccWindow *window);
bool ccPollEvent(ccWindow *window); //poll an event from the events that currently need to be processed in the window
void ccChangeWM(ccWindow *window, ccWindowMode mode);
void ccResizeMoveWindow(ccWindow *window, ccRect rect);
void ccCenterWindow(ccWindow *window);

//resolution
ccError ccSetResolution(ccDisplay *display, ccDisplayData *displayData);
bool ccResolutionExists(ccDisplay *display, ccDisplayData *resolution);

//display
void ccFindDisplays(); //get all displays currently connected and active
void ccUpdateDisplays(); //update the display list - TODO: update window display pointers
void ccFreeDisplays();
ccRect ccGetDisplayRect(ccDisplay *display);

int ccGetDisplayAmount();
ccDisplay *ccGetDisplay(int index);
ccDisplay *ccGetDefaultDisplay();

//opengl
ccError ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor); //bind the openGl context to window
void ccGLSwapBuffers(ccWindow *window); //swap the buffers
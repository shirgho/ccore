#pragma once

#include "../core.h"

#ifdef X11
#include "../../core_x11/interface/window.h"
#elif defined GTK
#include "../../core_gtk/interface/window.h"
#elif defined WIN32
#include "../../core_win32/interface/window.h"
#endif

//stores display properties
typedef struct {
	int width, height, refreshRate, bitDepth;
} ccDisplayData;

//an array of all possible display properties a display can acquire
typedef struct {
	ccDisplayData* displayData;
	int amount;
} ccResolutions;

//a display (often a monitor)
typedef struct {
	//current display configuration
	ccDisplayData currentDisplayData;
	int x, y;
	char gpuName[128];
	char monitorName[128];
	
#ifdef WIN32
	char deviceName[128];
#endif

} ccDisplay;

//list of all displays currently connected and active
typedef struct {
	ccDisplay* display; //READ ONLY!!
	int amount;
	int primary;
} ccDisplays;

//the way a window is shown. Only one window mode can be active at a time
typedef enum {
	CC_WINDOW_MODE_VISIBLE,
	CC_WINDOW_MODE_INVISIBLE,
	CC_WINDOW_MODE_FULLSCREEN,
	CC_WINDOW_MODE_MAXIMIZED, //Note: maximize from windowed mode
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
	Display *display; 
	Window window;
	int screen;
	GLXContext context;
#endif

#ifdef WIN32
	HDC hdc;
	MSG msg;
	HWND winHandle;
	HGLRC renderContext;
	bool sizeChanged;
#endif

} ccWindow;

ccWindow* ccNewWindow(ccRect rect, const char *title, int flags);
void ccFreeWindow(ccWindow *window);
//poll an event from the events that currently need to be processed in the window
bool ccPollEvent(ccWindow *window);
void ccChangeWM(ccWindow *window, ccWindowMode mode);
void ccResizeWindow(ccWindow *window, ccRect rect);
void ccCenterWindow(ccWindow *window); //Call only after setting WM to visible!

ccResolutions *ccGetResolutions(ccDisplay *display);
void ccFreeResolutions(ccResolutions *resolutions);
void ccSetResolution(ccDisplay *display, ccDisplayData *displayData);

//get all displays currently connected and active
void ccFindDisplays();
//update the display list
void ccUpdateDisplays(); //TODO: update window display pointers
//get a pointer to the display list
ccDisplays *ccGetDisplays();
//get the default display (E.G.for single monitor applications)
ccDisplay *ccGetDefaultDisplay();
void ccFreeDisplays();
void ccGetDisplayRect(ccDisplay *display, ccRect *rect);

//bind the openGl context to window
void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor);
//swap the buffers
void ccGLSwapBuffers(ccWindow *window);
//make window the current window
void ccGLMakeCurrent(ccWindow *window);

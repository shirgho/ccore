#pragma once

#include "../core.h"

#ifdef X11
#include "../../core_x11/interface/window.h"
#elif defined GTK
#include "../../core_gtk/interface/window.h"
#elif defined WIN32
#include "../../core_win32/interface/window.h"
#endif

typedef struct {
	int width, height, framerate;
} ccScreenDimension;

typedef struct {
	ccScreenDimension* resolutions;
	int nResolutions;
} ccResolutions;

typedef enum {
	CC_WINDOW_MODE_VISIBLE,
	CC_WINDOW_MODE_INVISIBLE,
	CC_WINDOW_MODE_FULLSCREEN,
	CC_WINDOW_MODE_MAXIMIZED, //Note: maximize from windowed mode
	CC_WINDOW_MODE_WINDOW,
	CC_WINDOW_MODE_MINIMIZED
} ccWindowMode;

typedef enum {
	CC_WINDOW_FLAG_NORESIZE = 1,
	CC_WINDOW_FLAG_ALWAYSONTOP = 2,
	CC_WINDOW_FLAG_NOBUTTONS = 4
} ccWindowFlag;

typedef struct _ccWindow {
	//Note: dimensions subtract border size
	float aspect;
	unsigned short width, height;
	ccPoint mouse;
	ccEvent event;

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

ccWindow* ccNewWindow(unsigned short width, unsigned short height, const char *title, int flags);
void ccFreeWindow(ccWindow *window);
bool ccPollEvent(ccWindow *window);
void ccChangeWM(ccWindow *window, ccWindowMode mode);

ccResolutions* ccGetResolutions(ccWindow *window);
void ccFreeResolutions(ccResolutions *resolutions);
void ccGetResolution(ccScreenDimension *dimensions); // Note: acts on primary display device on Win32
void ccSetResolution(ccScreenDimension *dimensions);

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor);
void ccGLSwapBuffers(ccWindow *window);
void ccGLMakeCurrent(ccWindow *window);

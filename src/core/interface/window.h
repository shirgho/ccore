#pragma once

#include "../core.h"

#ifdef X11
#include "../../core_x11/interface/window.h"
#elif defined GTK
#include "../../core_gtk/interface/window.h"
#elif defined WIN32
#include "../../core_win32/interface/window.h"
#endif

typedef enum {
	ccWMFullScreen,
	ccWMMaximized,
	ccWMWindow,
	ccWMMinimized
} ccWindowMode;

typedef enum {
	ccWFNoResize = 1,
	ccWFAlwaysOnTop = 2,
	ccWFNoButtons = 4
} ccWindowFlag;

typedef struct {
	//Note: dimensions subtract border size
	unsigned short width;
	unsigned short height;
	float aspect;
	ccEvent event;

#ifdef LINUX
	Display *display;
	Window window;
	int screen;
	GLXContext context;
#endif
} ccWindow;

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title, ccWindowMode mode, int flags);
void ccFreeWindow(ccWindow *window);
bool ccPollEvent(ccWindow *window);
void ccChangeWM(ccWindow *window, ccWindowMode mode);

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor);
void ccGLSwapBuffers(ccWindow *window);

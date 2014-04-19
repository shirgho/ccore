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
	ccWMMinimized //why would one want this?
} ccWindowMode;

typedef enum {
	ccWFNoResize,
	ccWFAlwaysOnTop
} ccWindowFlag;

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title, ccWindowMode mode);
void ccFreeWindow(ccWindow *window);
bool ccPollEvent(ccWindow *window);
void ccChangeWM(ccWindow *window);

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor);
void ccGLSwapBuffers(ccWindow *window);

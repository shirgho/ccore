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

#define CC_KEY_0 '0'
#define CC_KEY_1 '1'
#define CC_KEY_2 '2'
#define CC_KEY_3 '3'
#define CC_KEY_4 '4'
#define CC_KEY_5 '5'
#define CC_KEY_6 '6'
#define CC_KEY_7 '7'
#define CC_KEY_8 '8'
#define CC_KEY_9 '9'

#define CC_KEY_A 'A'
#define CC_KEY_B 'B'
#define CC_KEY_C 'C'
#define CC_KEY_D 'D'
#define CC_KEY_E 'E'
#define CC_KEY_F 'F'
#define CC_KEY_G 'G'
#define CC_KEY_H 'H'
#define CC_KEY_I 'I'
#define CC_KEY_J 'J'
#define CC_KEY_K 'K'
#define CC_KEY_L 'L'
#define CC_KEY_M 'M'
#define CC_KEY_N 'N'
#define CC_KEY_O 'O'
#define CC_KEY_P 'P'
#define CC_KEY_Q 'Q'
#define CC_KEY_R 'R'
#define CC_KEY_S 'S'
#define CC_KEY_T 'T'
#define CC_KEY_U 'U'
#define CC_KEY_V 'V'
#define CC_KEY_W 'W'
#define CC_KEY_X 'X'
#define CC_KEY_Y 'Y'
#define CC_KEY_Z 'Z'

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
void ccNewWindow(ccRect rect, const char *title, int flags);
void ccFreeWindow();
bool ccPollEvent(); //poll an event from the events that currently need to be processed in the window
void ccChangeWM(ccWindowMode mode);
void ccResizeMoveWindow(ccRect rect, bool addBorder); //addBorder indicates whether the border size is included in the rect or whether it has to be added
void ccCenterWindow();
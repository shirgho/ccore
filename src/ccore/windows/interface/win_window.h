#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <hidsdi.h>
#include <stdint.h>

#include <ccore/window.h>

#include <ccore/string.h>
#include <ccore/assert.h>
#include <ccore/gamepad.h>

#include "../utils/win_file.h"
#include "win_gamepad.h"

#ifdef CC_USE_GAMEPAD
#define NRAWINPUTDEVICES 3
#define RAWINPUT_GAMEPAD 2
#define RAWINPUT_GAMEPADCOUNT 1
#else
#define NRAWINPUTDEVICES 2
#define RAWINPUT_GAMEPADCOUNT 0
#endif

#define RAWINPUT_KEYBOARD 0
#define RAWINPUT_MOUSE 1

typedef struct {
	HDC hdc;
	MSG msg;
	HWND winHandle;
	HGLRC renderContext;
	LONG style;
	RAWINPUTDEVICE rid[NRAWINPUTDEVICES];
	LPBYTE lpb;
	UINT lpbSize;
	UINT dwSize;
#ifdef CC_USE_GAMEPAD
	bool queryXinput;
#endif

	ccEvent *eventStack;
	int eventStackSize;
	int eventStackPos;
	int eventStackIndex;

	ccCursor cursor;

	int flags;
} ccWindow_win;

void _ccEventStackPush(ccEvent event);

#define WINDOW_DATA ((ccWindow_win*)_ccWindow->data)
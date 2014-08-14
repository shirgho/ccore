#pragma once

#include <stdlib.h>
#include <Windows.h>

#include <ccore/window.h>

#include <ccore/charUtils.h>
#include <ccore/assert.h>
#include <ccore/gamepad.h>

#define NRAWINPUTDEVICES 3
#define RAWINPUT_KEYBOARD 0
#define RAWINPUT_MOUSE 1
#define RAWINPUT_GAMEPAD 2

typedef enum {
	CC_WIN32_EVENT_RESIZED = 1,
	CC_WIN32_EVENT_FOCUS_GAINED = 2,
	CC_WIN32_EVENT_FOCUS_LOST = 4
} ccWin32Event;

typedef struct {
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
} ccWindow_win;

#define WINDOW_DATA ((ccWindow_win*)_window->data)
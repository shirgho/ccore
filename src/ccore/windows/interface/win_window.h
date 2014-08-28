#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <Dbt.h>

#include <ccore/window.h>

#include <ccore/charUtils.h>
#include <ccore/assert.h>
#include <ccore/gamepad.h>

#include "win_gamepad.h"

#define NRAWINPUTDEVICES 3
#define RAWINPUT_KEYBOARD 0
#define RAWINPUT_MOUSE 1
#define RAWINPUT_GAMEPAD 2

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
	bool queryXinput;

	ccEvent *eventStack;
	int eventStackSize;
	int eventStackPos;
	int eventStackIndex;
} ccWindow_win;

void _ccEventStackPush(ccEvent event);

#define WINDOW_DATA ((ccWindow_win*)_window->data)
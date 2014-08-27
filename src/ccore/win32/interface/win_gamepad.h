#pragma once

#include <Windows.h>
#include <hidsdi.h>
#include <Xinput.h>

#include <ccore/gamepad.h>

#include <ccore/assert.h>

#include "win_window.h"

#define GAMEPAD_MAXBUTTONS 128

void _generateGamepadEvents(RAWINPUT *raw);
void _queryXinput();

typedef struct {
	HIDP_CAPS caps;
	PHIDP_BUTTON_CAPS buttonCaps;
	PHIDP_VALUE_CAPS valueCaps;
	PHIDP_PREPARSED_DATA preparsedData;
	HANDLE handle;
	int preparsedDataSize;
	int *axisNegativeComponent;
	double *axisFactor;
} ccGamepad_win;

typedef struct {
	USAGE usage[GAMEPAD_MAXBUTTONS];
} ccGamepads_win;

#define GAMEPAD_DATA ((ccGamepad_win*)currentGamepad->data)
#define GAMEPADS_DATA ((ccGamepads_win*)_gamepads->data)
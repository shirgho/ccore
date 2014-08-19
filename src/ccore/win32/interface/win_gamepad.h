#pragma once

#include <Windows.h>
#include <hidsdi.h>

#include <ccore/gamepad.h>

#include <ccore/assert.h>

#include "win_window.h"

#define GAMEPAD_MAXBUTTONS 128
#define GAMEPAD_MAXVALUES 128

ccGamepadEvent _generateGamepadEvent(RAWINPUT *raw);

typedef struct {
	HIDP_CAPS caps;
	PHIDP_BUTTON_CAPS buttonCaps;
	PHIDP_VALUE_CAPS valueCaps;
} ccGamepad_win;

typedef struct {
	PHIDP_PREPARSED_DATA preparsedData;
} ccGamepads_win;

#define GAMEPAD_DATA ((ccGamepad_win*)currentGamepad->data)
#define GAMEPADS_DATA ((ccGamepads_win*)_gamepads->data)
#pragma once

#include <Windows.h>

#include <ccore/gamepad.h>

#include <ccore/assert.h>

#include "win_window.h"

typedef struct {
	UINT id;
} ccGamepad_win;

#define GAMEPAD_DATA(gamepad) ((ccGamepad_win*)gamepad->data)
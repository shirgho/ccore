#pragma once

#include <stdlib.h>
#include <Windows.h>

#include <ccore/charUtils.h>
#include <ccore/assert.h>

#define NRAWINPUTDEVICES 2
#define RAWINPUT_KEYBOARD 0
#define RAWINPUT_MOUSE 1

typedef enum {
	CC_WIN32_EVENT_RESIZED = 1,
	CC_WIN32_EVENT_FOCUS_GAINED = 2,
	CC_WIN32_EVENT_FOCUS_LOST = 4
} ccWin32Event;
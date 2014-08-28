//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                //
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License as published by        //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

#pragma once

#include <limits.h>

#include "core.h"
#include "error.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GAMEPAD_AXIS_MIN SHRT_MIN
#define GAMEPAD_AXIS_MAX SHRT_MAX

#define GAMEPAD_HAPTIC_FORCE_MIN 0
#define GAMEPAD_HAPTIC_FORCE_MAX USHRT_MAX

typedef enum {
	CC_GAMEPAD_UNHANDLED = 0,
	CC_GAMEPAD_BUTTON_DOWN,
	CC_GAMEPAD_BUTTON_UP,
	CC_GAMEPAD_AXIS_MOVE,
	CC_GAMEPAD_CONNECT,
	CC_GAMEPAD_DISCONNECT
} ccGamepadEventType;

typedef struct {
	ccGamepadEventType type;
	
	union {
		int buttonId;
		int axisId;
	};

	int id;
} ccGamepadEvent;

typedef struct {
	char *name;
	bool *button;
	int *axis;

	int axisAmount, buttonAmount, hapticAmount;

	bool plugged;

	void *data;
} ccGamepad;

typedef struct {
	ccGamepad *gamepad;
	int amount;

	void *data;
} ccGamepads;

ccGamepads *_gamepads;

#define ccGamepadCount() _gamepads->amount
#define ccGamepadGet(index) (&_gamepads->gamepad[index])

ccError ccGamepadInitialize(void); // Initializes gamepad input
ccError ccGamepadHapticSet(ccGamepad *gamepad, int hapticIndex, int force);
void ccGamepadFree(void); // Stops gamepad input

#ifdef __cplusplus
}
#endif

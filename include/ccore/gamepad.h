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
//      it under the terms of the GNU Lesser General Public License as published by //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU Lesser General Public License    //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

#ifdef CC_USE_GAMEPAD

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

#define GAMEPAD_OUTPUT_VALUE_MIN 0
#define GAMEPAD_OUTPUT_VALUE_MAX USHRT_MAX

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
	int *output;
	bool plugged;

	int axisAmount, buttonAmount, outputAmount;

	void *data;
} ccGamepad;

typedef struct {
	ccGamepad *gamepad;
	int amount;

	void *data;
} ccGamepads;

ccGamepads *_ccGamepads;

#define ccGamepadCount() _ccGamepads->amount
#define ccGamepadGet(index) (&_ccGamepads->gamepad[index])

ccReturn ccGamepadInitialize(void); // Initializes gamepad input
ccReturn ccGamepadFree(void); // Stops gamepad input
ccReturn ccGamepadOutputSet(ccGamepad *gamepad, int outputIndex, int force);

#ifdef __cplusplus
}
#endif

#else
#define CC_USE_GAMEPAD was not defined
#endif

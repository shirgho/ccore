//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.1                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (job@ccore.org)                       //
//                                 \ Thomas Versteeg (thomas@ccore.org)             //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the 3-clause BSD license.                             //
//                                                                                  //
//      You should have received a copy of the 3-clause BSD License along with      //
//      this program. If not, see <http://opensource.org/licenses/>.                //
//__________________________________________________________________________________//

#if defined CC_USE_ALL || defined CC_USE_GAMEPAD

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
	wchar_t *name;
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

#elif defined __GNUC__
#error "The CC_USE_GAMEPAD or the CC_USE_ALL flag must be set"
#endif

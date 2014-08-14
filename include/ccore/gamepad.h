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

#include "core.h"
#include "error.h"
#include "types.h"

typedef enum {
	CC_GAMEPAD_UNHANDLED = 0,
	CC_GAMEPAD_BUTTON_DOWN,
	CC_GAMEPAD_BUTTON_UP,
	CC_GAMEPAD_JOYSTICK_MOVE
} ccGamepadEventType;

typedef struct {
	ccGamepadEventType type;
	
	ccPoint joystickPosition;
	unsigned int buttonId;
	unsigned int gamepadId;
} ccGamepadEvent;

typedef struct {
	char *name;
	int axisAmount, buttonsAmount;

	void *data;
} ccGamepad;

typedef struct {
	ccGamepad* gamepad;
	unsigned int amount;
} ccGamepads;

ccGamepads *_gamepads;

ccError ccGamepadConnect(); // Connects or reconnects gamepads
int ccGamepadCount();
void ccGamepadDisconnect(); // Disconnect any connected gamepads

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

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	CC_MOUSE_UNHANDLED = 0,
	CC_MOUSE_MOVE,
	CC_MOUSE_BUTTON_UP,
	CC_MOUSE_BUTTON_DOWN,
	CC_MOUSE_CONNECT,
	CC_MOUSE_DISCONNECT
} ccMouseEventType;

typedef struct {
	ccMouseEventType type;
	
	union {
		int buttonId;
		ccPoint vector;
	};

	int id;
} ccMouseEvent;

typedef struct {
	char *name;
	bool *button;

	int buttonAmount, id;

	bool plugged;

	void *data;
} ccMouse;

typedef struct {
	ccMouse* mouse;
	int amount;

	void *data;
} ccMice;

ccMice *_mice;

#define ccMouseCount() _mice->amount
#define ccMouseGet(index) _mice->mouse[index]

ccError ccMouseInitialize(); // Connects mice
ccError ccMouseRefresh(); // Reconnects mice
void ccMouseFree(); // Disconnect any connected mice

#ifdef __cplusplus
}
#endif

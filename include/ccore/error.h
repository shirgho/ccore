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

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	CC_ERROR_NONE = 0, // No errors

	// Display related
	CC_ERROR_NODISPLAY, // The window couldn't find a display to attach to
	CC_ERROR_RESOLUTION_CHANGE, // Resolution change failed

	// Window related
	CC_ERROR_WINDOWCREATION, // The window can't be created
	CC_ERROR_WINDOWDESTRUCTION, // The window can't be destroyed
	CC_ERROR_WINDOW_MODE, // The window mode couldn't be changed (also moving & resolution)

	// OpenGL related
	CC_ERROR_GLVERSION, // The target OpenGL version is not supported
	CC_ERROR_GLCONTEXT, // OpenGL context creation failed
	CC_ERROR_GLEWINIT, // GLEW could not initialize
	CC_ERROR_CANTSWAP, // The buffers couldn't swap

	// Thread related
	CC_ERROR_THREAD, // CCORE couldn't start a thread
	CC_ERROR_MUTEX_CREATION, // A mutex object couldn't be created
	CC_ERROR_MUTEX, // Error working with a mutex

	// Networking related
	CC_ERROR_NET,

	// Gamepad related
	CC_ERROR_NOGAMEPAD, // No gamepads could be found
	CC_ERROR_GAMEPADDATA, // The gamepad could not be read
	CC_ERROR_NOHAPTIC, // This motor is not accessible

	// Mouse related
	CC_ERROR_NOMOUSE, // No mice could be found
	CC_ERROR_MOUSEDATA, // The mice could not be read

	// Memory related
	CC_ERROR_OUT_OF_MEMORY
} ccError;

// The following macro's can be used inside functions that return ccResult to catch allocation failures

#define ccMalloc(x, size) { \
	x = malloc(size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\

#define ccCalloc(x, amount, size) { \
	x = calloc(amount, size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\

#define ccRealloc(x, size) { \
	x = realloc(x, size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\

const char *ccErrorString(ccError error);
void ccErrorPush(ccError error);
ccError ccErrorPop(void);

void _ccErrorFree(void);

#ifdef __cplusplus
}
#endif

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

#include "print.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	CC_ERROR_NONE = 0, // No errors

	// Global
	CC_ERROR_INVALID_ARGUMENT,

	// Display related
	CC_ERROR_DISPLAY_NONE, // The window couldn't find a display to attach to
	CC_ERROR_DISPLAY_RESOLUTIONCHANGE, // Resolution change failed

	// Window related
	CC_ERROR_WINDOW_NONE,
	CC_ERROR_WINDOW_CREATE, // The window can't be created
	CC_ERROR_WINDOW_DESTROY, // The window can't be destroyed
	CC_ERROR_WINDOW_MODE, // The window mode couldn't be changed (also moving & resolution)

	// OpenGL related
	CC_ERROR_GL_VERSION, // The target OpenGL version is not supported
	CC_ERROR_GL_CONTEXT, // OpenGL context creation failed
	CC_ERROR_GL_GLEWINIT, // GLEW could not initialize
	CC_ERROR_GL_BUFFERSWAP, // The buffers couldn't swap

	// Thread related
	CC_ERROR_THREAD_CREATE, // CCORE couldn't start a thread
	CC_ERROR_THREAD_MUTEXCREATE, // A mutex object couldn't be created
	CC_ERROR_THREAD_MUTEX, // Error working with a mutex

	// Networking related
	CC_ERROR_NET, //TODO: not implemented in wiki

	// Gamepad related
	CC_ERROR_GAMEPAD_NONE, // No gamepads could be found
	CC_ERROR_GAMEPAD_DATA, // The gamepad could not be read
	CC_ERROR_GAMEPAD_HAPTICNONE, // This motor is not accessible

	// Mouse related
	CC_ERROR_MOUSE_NONE, // No mice could be found
	CC_ERROR_MOUSE_DATA, // The mice could not be read

	// Memory related
	CC_ERROR_MEMORY_OVERFLOW,

	// File related
	CC_ERROR_FILE_OPEN, // Error opening the file
} ccError;

// The following macro's can be used inside functions that return ccResult to catch allocation failures

#define ccMalloc(x, size) { \
	x = malloc(size); \
	if(x == NULL) { \
		ccErrorPush(CC_ERROR_MEMORY_OVERFLOW); \
		return CC_FAIL; \
		} \
	} \

#define ccCalloc(x, amount, size) { \
	x = calloc(amount, size); \
	if(x == NULL) { \
		ccErrorPush(CC_ERROR_MEMORY_OVERFLOW); \
		return CC_FAIL; \
		} \
	} \

#define ccRealloc(x, size) { \
	x = realloc(x, size); \
	if(x == NULL) { \
		ccErrorPush(CC_ERROR_MEMORY_OVERFLOW); \
		return CC_FAIL; \
		} \
	} \

const char *ccErrorString(ccError error);
void _ccErrorPush(ccError error, char *file, int line);
#define ccErrorPush(error) _ccErrorPush(error, __FILE__, __LINE__)
ccError ccErrorPop(void);

void _ccErrorFree(void);

#ifdef __cplusplus
}
#endif

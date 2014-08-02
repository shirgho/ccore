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

typedef enum {
	CC_ERROR_NONE = 0, //No errors

	//Display related
	CC_ERROR_NODISPLAY, //The window couldn't find a display to attach to
	CC_ERROR_RESOLUTION_CHANGE, //Resolution change failed

	//Window related
	CC_ERROR_WINDOWCREATION, //The window can't be created
	CC_ERROR_WINDOWDESTRUCTION, //The window can't be destroyed
	CC_ERROR_WINDOW_MODE, //The window mode couldn't be changed (also moving & resolution)

	//OpenGL related
	CC_ERROR_GLVERSION, //The target OpenGL version is not supported
	CC_ERROR_GLCONTEXT, //OpenGL context creation failed
	CC_ERROR_GLEWINIT, //GLEW could not initialize
	CC_ERROR_CANTSWAP, //The buffers couldn't swap

	CC_ERROR_OUT_OF_MEMORY //Out of memory exception
} ccError;

//The following macro's can be used inside functions that return ccError to catch allocation failures

#define ccMalloc(x, size) { \
	x = malloc(size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\

#define ccRealloc(x, size) { \
	x = realloc(x, size); \
	if(x == NULL) return CC_ERROR_OUT_OF_MEMORY; \
	}\
	
const char *ccErrorString(ccError error);

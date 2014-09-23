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

#pragma once

#include "core.h"

#include "window.h"
#include "error.h"
#include "types.h"

#ifdef LINUX
#include <GL/glew.h>
#elif defined WINDOWS
#include "../../lib/glew/GL/glew.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
// opengl
ccReturn ccGLContextBind(int glVersionMajor, int glVersionMinor); // Bind the openGl context to window
ccReturn ccGLContextFree(void); // Free context
ccReturn ccGLBuffersSwap(void); // Swap the buffers
bool ccGLContextIsActive(void);

#ifdef __cplusplus
}
#endif

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
#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* ccThread;

#define ccThreadReturn() return 0;

#ifdef WINDOWS
#include <windows.h>
#define ccThreadFunction(name) DWORD WINAPI name(LPVOID lpParam)
#define ccThreadData ((void*)lpParam)
#elif defined X11
#define ccThreadFunction(name) void* name(void *arg)
#define ccThreadData arg
#endif

ccReturn ccThreadCreate(ccThread *thread, void *function);
ccReturn ccThreadStart(ccThread thread, void *data);
ccReturn ccThreadJoin(ccThread thread);
bool ccThreadFinished(ccThread thread);

#ifdef __cplusplus
}
#endif

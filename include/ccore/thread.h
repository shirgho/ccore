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

#ifdef CC_USE_THREAD

#include "core.h"
#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WINDOWS

#include <windows.h>

#define ccThreadFunction(name) DWORD WINAPI name(LPVOID lpParam)
#define ccThreadData ((void*)lpParam)

#define ccThreadReturn() return 0;

typedef HANDLE ccThread;
typedef CRITICAL_SECTION ccMutex;

#elif defined LINUX

#include <pthread.h>

#define ccThreadFunction(name) void* name(void *arg)
#define ccThreadData arg

#define ccThreadReturn() return 0;

typedef pthread_t ccThread;
typedef pthread_mutex_t ccMutex;

#endif

ccReturn ccThreadStart(ccThread *thread, void *function, void *data);
ccReturn ccThreadJoin(ccThread *thread);
bool ccThreadFinished(ccThread *thread);

ccReturn ccThreadMutexCreate(ccMutex *mutex);
ccReturn ccThreadMutexJoin(ccMutex *mutex);
ccReturn ccThreadMutexRelease(ccMutex *mutex);
ccReturn ccThreadMutexFree(ccMutex *mutex);

#ifdef __cplusplus
}
#endif

#endif
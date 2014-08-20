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

#include <stdint.h>

#include "core.h"

#define _TO_SECONDS 1000000000LL
#define _TO_MILLISECONDS 1000000LL
#define _TO_MICROSECONDS 1000LL

#define ccTimeSeconds() (ccTimeNanoseconds() / _TO_SECONDS)
#define ccTimeMilliseconds() (ccTimeNanoseconds() / _TO_MILLISECONDS)
#define ccTimeMicroseconds() (ccTimeNanoseconds() / _TO_MICROSECONDS)

#ifdef __cplusplus
extern "C"
{
#endif

void ccTimeDelay(int ms);
uint64_t ccTimeNanoseconds();

#ifdef __cplusplus
}
#endif

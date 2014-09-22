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

#ifdef __linux__

#define X11
#define LINUX

#elif defined _WIN32

#define WINDOWS

#else
#error "OS not supported!"
#endif

#ifdef __GNUC__
#define CC_LIKELY(x) __builtin_expect(!!(x), 1)
#define CC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CC_LIKELY(x) (x)
#define CC_UNLIKELY(x) (x)
#endif

void ccInitialize(void); // Initialize ccore
void ccFree(void); // Free ccore

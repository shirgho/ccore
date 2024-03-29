//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (job@ccore.org)                       //
//                                 \ Thomas Versteeg (thomas@ccore.org)             //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the 3-clause BSD license.                             //
//                                                                                  //
//      You should have received a copy of the 3-clause BSD License along with      //
//      this program. If not, see <http://opensource.org/licenses/>.                //
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

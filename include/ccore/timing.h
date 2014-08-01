//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___|                               //
//                              \______\                                            //
//                                                                                  //
//              Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)               //
//                                  \ Thomas Versteeg (thomasversteeg@gmx.com)      //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
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
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core.h"

#ifdef LINUX
#include "../../src/ccore/linux/utils/timing.h"
#elif defined WIN32
#include "../../src/ccore/win32/utils/timing.h"
#endif

void ccDelay(int ms);

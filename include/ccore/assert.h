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

#include "core.h"

//ccAssert translates to assert when debugging, it is ignored otherwise.

#ifdef _DEBUG
#include <assert.h>
#define ccAssert(x) assert(x)
#else
#define ccAssert(x) 
#endif

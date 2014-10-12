//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.1                           //
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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

void ccStringTrimToChar(wchar_t *str, wchar_t ch, bool includeChar);
void ccStringReplaceChar(wchar_t *str, wchar_t ch, wchar_t newCh);
wchar_t *ccStringConcatenate(int amount, ...);

#ifdef __cplusplus
}
#endif

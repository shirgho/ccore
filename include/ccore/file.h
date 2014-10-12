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

#if defined CC_USE_ALL || defined CC_USE_FILE

#include <stdint.h>
#include <sys/stat.h>

#include "core.h"
#include "error.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	uint64_t size;
	time_t modified;
	time_t access;
} ccFileInfo;

// These functions can be used to get OS specific directories to store program data
wchar_t *ccFileUserDirGet(void);
wchar_t *ccFileDataDirGet(void);
wchar_t *ccFileTempDirGet(void);

ccFileInfo ccFileInfoGet(wchar_t *file); 

void _ccFileFree(void);

#ifdef __cplusplus
}
#endif

#elif defined __GNUC__
#error "The CC_USE_FILE or the CC_USE_ALL flag must be set"
#endif

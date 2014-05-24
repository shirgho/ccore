#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../core_x11/utils/timing.h"
#elif defined WIN32
#include "../../core_win32/utils/timing.h"
#endif

char *ccGetUserDir();
char *ccGetAppDir();
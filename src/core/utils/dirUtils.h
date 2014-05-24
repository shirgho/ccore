#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../core_x11/utils/dirUtils.h"
#elif defined WIN32
#include "../../core_win32/utils/dirUtils.h"
#endif

char *ccGetUserDir();
char *ccGetDataDir();
char *ccGetTempDir();
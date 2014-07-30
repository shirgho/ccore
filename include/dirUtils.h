#pragma once

#include "core.h"

#ifdef LINUX
#include "../src/linux/utils/dirUtils.h"
#elif defined WIN32
#include "../src/win32/utils/dirUtils.h"
#endif

char *ccGetUserDir();
char *ccGetDataDir();
char *ccGetTempDir();
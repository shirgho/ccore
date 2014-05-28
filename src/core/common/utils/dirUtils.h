#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../linux/utils/dirUtils.h"
#elif defined WIN32
#include "../../win32/utils/dirUtils.h"
#endif

char *ccGetUserDir();
char *ccGetDataDir();
char *ccGetTempDir();
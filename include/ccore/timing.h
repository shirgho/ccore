#pragma once

#include "core.h"

#ifdef LINUX
#include "../../src/ccore/linux/utils/timing.h"
#elif defined WIN32
#include "../../src/ccore/win32/utils/timing.h"
#endif

void ccDelay(int ms);

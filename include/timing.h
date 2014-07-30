#pragma once

#include "core.h"

#ifdef LINUX
#include "../src/linux/utils/timing.h"
#elif defined WIN32
#include "../src/win32/utils/timing.h"
#endif

void ccDelay(int ms);
